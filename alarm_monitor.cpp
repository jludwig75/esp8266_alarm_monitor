#include "alarm_monitor.h"

#include "sms.h"
#include "get_time.h"
#include "status_led.h"
#include "simple_queue.h"
#include "logger.h"

#include <Arduino.h>

namespace
{

int alarmInterruptPin = __INT16_MAX__;

struct InterruptEvent
{
    int type;
    unsigned long millis;
};

SimpleQueue<InterruptEvent, 1024> interruptEvents;

unsigned long lastInterruptChangeTime = millis();
int lastInterruptChange = FALLING;
int lastInterruptLevel = LOW;
const unsigned long minimumAlarmSoundingTimeMiliseconds = 500;
const unsigned long minimumAlarmReportIntervalMiliseconds = 30 * 1000;

ICACHE_RAM_ATTR void onAlarmInterrupt()
{
    unsigned long interruptTime = millis();
    int currentLevel = digitalRead(alarmInterruptPin);

    noInterrupts();
    if (currentLevel == HIGH)
    {
        if (lastInterruptLevel == LOW)
        {
            lastInterruptLevel = HIGH;
            lastInterruptChange = RISING;
            lastInterruptChangeTime = interruptTime;
            InterruptEvent evt;
            evt.type = 1;
            evt.millis = interruptTime;
            interruptEvents.push(evt);
        }
    }
    else
    {
        if (lastInterruptLevel == HIGH)
        {
            lastInterruptLevel = LOW;
            lastInterruptChange = FALLING;
            lastInterruptChangeTime = interruptTime;
            InterruptEvent evt;
            evt.millis = interruptTime;
            evt.type = 0;
            interruptEvents.push(evt);
        }
    }
    interrupts();
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

unsigned long lastReportTime = millis() - minimumAlarmReportIntervalMiliseconds;

void alarm_monitor_on_loop()
{
    unsigned long now = millis();
    if (lastInterruptChange == RISING &&
        now - lastInterruptChangeTime >= minimumAlarmSoundingTimeMiliseconds &&
        now - lastReportTime >= minimumAlarmReportIntervalMiliseconds)
    {
        status_led_on();

        logger.log(String("Sending alert message at ") + String(millis()) + "...");
        String message("Your home alarm system was triggered at ");
        message += get_date_time_string();
        
        if (send_sms("+18013600861", message.c_str()))
        {
            lastReportTime = now;
        }

        status_led_off();
    }

    log_interrupt_event();
}
