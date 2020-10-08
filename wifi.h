#pragma once

#include <string.h>

#include <ESP8266WiFiMulti.h>


class Wifi
{
  public:
    Wifi();
    void begin(const char* ssid, const char* password);
    bool connected() const;
    const String& ssid() const;
    const IPAddress& ip_address() const;
  private:
    bool _connected;
    String _ssid;
    IPAddress _ip_address;
    ESP8266WiFiMulti _wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
};
