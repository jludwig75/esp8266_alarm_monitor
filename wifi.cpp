#include "wifi.h"

#include <ESP8266WiFi.h>


Wifi::Wifi()
  :
  _connected(false)
{
}

void Wifi::begin(const char* ssid, const char* password)
{
    _wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to
    //wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
    //wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

    Serial.println("Connecting ...");
    int i = 0;
    while (_wifiMulti.run() != WL_CONNECTED)
    { // Wait for the Wi-Fi to connect
        delay(250);
        Serial.print('.');
    }
    Serial.println('\n');
    _connected = true;
    _ssid = WiFi.SSID();
    _ip_address = WiFi.localIP();
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());              // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
}


bool Wifi::connected() const
{
    return _connected;
}


const String& Wifi::ssid() const
{
    return _ssid;
}


const IPAddress& Wifi::ip_address() const
{
    return _ip_address;
}
