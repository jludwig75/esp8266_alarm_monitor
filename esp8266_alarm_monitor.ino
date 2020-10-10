/*
 * IMPORTANT: Select Flash Size 4MB OTA:~1019KB
 * -- JRL
 */
#include <Arduino.h>
#include "wifi.h"
#include "ota.h"
#include "status_led.h"
#include "alarm_monitor.h"
#include "config.h"
#include "logger.h"

const int ALARM_PIN = 14;
const int LED_PIN = LED_BUILTIN;

Wifi wifi;

void setup()
{
    Serial.begin(115200);
    delay(10);
    Serial.println('\n');

    wifi.begin(WIFI_SSID, WIFI_PASSWORD);
    logger.begin();
    ota_setup();
    status_led_begin(LED_PIN);
    alarm_monitor_begin(ALARM_PIN);
}

void loop()
{
    ota_on_loop();
    status_led_on_loop();
    alarm_monitor_on_loop();
    logger.on_loop();
}
