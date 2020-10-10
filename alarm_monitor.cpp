#include "alarm_monitor.h"

#include "sms.h"
#include "get_time.h"
#include "status_led.h"
#include "simple_queue.h"
#include "logger.h"

#include <Arduino.h>

namespace
{

bool sendAlert = false;
int alarmInterruptPin = __INT16_MAX__;

struct InterruptEvent
{
    int type;
    unsigned long millis;
};

SimpleQueue<InterruptEvent, 1024> interruptEvents;

ICACHE_RAM_ATTR void onAlarmInterrupt()
{
    if (digitalRead(alarmInterruptPin) == HIGH)
    {
        InterruptEvent evt;
        evt.type = RISING;
        evt.millis = millis();

        interruptEvents.push(evt);

        Serial.println("Alarm sounded!");
        sendAlert = true;
    }
    else
    {
        InterruptEvent evt;
        evt.type = FALLING;
        evt.millis = millis();

        interruptEvents.push(evt);
    }
}

void log_interrupt_event()
{
    noInterrupts();

    if (interruptEvents.empty())
    {
        interrupts();
        return;
    }

    InterruptEvent evt;
    interruptEvents.pop(evt);
    interrupts();

    //String message = String("Interrupt: ") + " millis: " + String(evt.millis) + " type: " + String(evt.type);
    String message = String(evt.millis) + "," + String(evt.type);
    logger.log(message);
}

}   // namespace

void alarm_monitor_begin(int alarm_monitor_pin)
{
    alarmInterruptPin = alarm_monitor_pin;
    sms_begin();
    get_time_begin();

    // Enable an external interrupt on the alarm sounder line
    pinMode(alarm_monitor_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(alarm_monitor_pin), onAlarmInterrupt, CHANGE);
}

void alarm_monitor_on_loop()
{
    if (sendAlert)
    {
        status_led_on();

        logger.log("Sending alert message...");
        String message("You home alarm system was triggered at ");
        message += get_date_time_string();
        
        send_sms("+18013600861", message.c_str());

        sendAlert = false;

        status_led_off();
    }

    log_interrupt_event();
}
