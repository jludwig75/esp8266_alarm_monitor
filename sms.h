#pragma once

void sms_begin();

bool send_sms(const char* to_number, const char* message);
