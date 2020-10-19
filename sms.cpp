#include "sms.h"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include "twilio.hpp"
#include "config.h"

namespace
{
// Find the api.twilio.com SHA1 fingerprint, this one was valid as 
// of July 2020. This will change, please see 
// https://www.twilio.com/docs/sms/tutorials/how-to-send-sms-messages-esp8266-cpp
// to see how to update the fingerprint.
const char fingerprint[] = TWILIO_FINGER_PRINT;

// Twilio account specific details, from https://twilio.com/console
// Please see the article: 
// https://www.twilio.com/docs/guides/receive-and-reply-sms-and-mms-messages-esp8266-c-and-ngrok

// If this device is deployed in the field you should only deploy a revocable
// key. This code is only suitable for prototyping or if you retain physical
// control of the installation.
const char* account_sid = TWILIO_SID;
const char* auth_token = TWILIO_AUTH_TOKEN;

// Details for the SMS we'll send with Twilio.  Should be a number you own 
// (check the console, link above).
String from_number = "+13852637908";

// Global twilio objects
Twilio twilio(account_sid, auth_token, fingerprint);

} // namespace

void sms_begin()
{
}

bool send_sms(const char* to_number, const char* message)
{
    String response;
    return twilio.send_message(to_number, from_number, message, response, "");
}
