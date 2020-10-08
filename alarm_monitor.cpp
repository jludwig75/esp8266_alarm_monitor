#include "alarm_monitor.h"

#include "sms.h"
#include "get_time.h"
#include "status_led.h"

#include <Arduino.h>

namespace
{

bool sendAlert = false;

ICACHE_RAM_ATTR void onAlarm()
{
    Serial.println("Alarm sounded!");
    sendAlert = true;
}

}   // namespace

void alarm_monitor_begin(int alarm_monitor_pin)
{
    sms_begin();
    get_time_begin();

    // Enable an external interrupt on the alarm sounder line
    pinMode(alarm_monitor_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(alarm_monitor_pin), onAlarm, RISING);
}

void alarm_monitor_on_loop()
{
    if (sendAlert)
    {
        status_led_on();

        //unsigned short level = analogRead(A0);

        Serial.println("Sending alert message...");
        String message("You home alarm system was triggered at ");
        message += get_date_time_string();
        //message += " voltage level";
        //message += level;
        
        send_sms("+18013600861", message.c_str());

        sendAlert = false;

        status_led_off();
    }
}
