#pragma once

#include <Arduino.h>


class Logger
{
public:
    void begin();
    void on_loop();
    void log(const String& log_message);
};


extern Logger logger;
