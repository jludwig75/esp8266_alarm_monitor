#include "status_led.h"

#include <Arduino.h>


namespace
{

int led_pin_number = __INT16_MAX__;

unsigned long previousTime = millis();
const unsigned long interval = 500;

}

void status_led_begin(int pin_number)
{
    led_pin_number = pin_number;
    pinMode(led_pin_number, OUTPUT);
    digitalWrite(led_pin_number, 1);
}

void status_led_on_loop()
{
    unsigned long diff = millis() - previousTime;
    if(diff > interval)
    {
        digitalWrite(led_pin_number, !digitalRead(led_pin_number));  // Change the state of the LED
        previousTime += diff;

        if (digitalRead(led_pin_number))
        {
            Serial.print("+");
        }
    }
}

void status_led_on()
{
    digitalWrite(led_pin_number, LOW);
}

void status_led_off()
{
    digitalWrite(led_pin_number, HIGH);
}