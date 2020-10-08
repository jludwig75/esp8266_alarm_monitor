/*
 * IMPORTANT: Select Flash Size 4MB OTA:~1019KB
 * -- JRL
 */
#include <Arduino.h>
#include "wifi.h"
#include "ota.h"
#include "sms.h"
#include "get_time.h"
#include "config.h"

Wifi wifi;

const byte led = LED_BUILTIN;

//#define TEST_MODE

#ifdef TEST_MODE

#define ALARM_PIN 0
#define ALARM_LOGIC_CHANGE  FALLING

#else   // TEST_MODE

#define ALARM_PIN 14
#define ALARM_LOGIC_CHANGE  RISING

#endif  // TEST_MODE

bool sendAlert = false;

ICACHE_RAM_ATTR void onAlarm()
{
    Serial.println("Alarm sounded!");
    sendAlert = true;
}

void setup()
{
    Serial.begin(115200);         // Start the Serial communication to send messages to the computer
    delay(10);
    Serial.println('\n');

    wifi.begin(WIFI_SSID, WIFI_PASSWORD);
    ota_setup();
    sms_begin();
    get_time_begin();

    pinMode(led, OUTPUT);
    digitalWrite(led, 1);

    // Enable an external interrupt on the alarm sounder line
    pinMode(ALARM_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ALARM_PIN), onAlarm, ALARM_LOGIC_CHANGE);
}

unsigned long previousTime = millis();

const unsigned long interval = 500;

void loop()
{
    ota_on_loop();
    unsigned long diff = millis() - previousTime;
    if(diff > interval)
    {
        digitalWrite(led, !digitalRead(led));  // Change the state of the LED
        previousTime += diff;

        if (digitalRead(led))
        {
            Serial.print("+");
        }
    }

    if (sendAlert)
    {
        digitalWrite(led, LOW);

        //unsigned short level = analogRead(A0);

        Serial.println("Sending alert message...");
        String message("You home alarm system was triggered at ");
        message += get_date_time_string();
        //message += " voltage level";
        //message += level;
        
        send_sms("+18013600861", message.c_str());

        sendAlert = false;

        digitalWrite(led, HIGH);
    }
}
