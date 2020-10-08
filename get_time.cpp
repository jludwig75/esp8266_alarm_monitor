#include "get_time.h"

#include <Timezone.h>

#include "ntp_client.h"


namespace
{

NtpClient ntp;
TimeChangeRule myDST = { "MDT", Second, Sun, Mar, 2, -6 * 60 };    //Daylight time = UTC - 6 hours
TimeChangeRule mySTD = { "MST", First, Sun, Nov, 2, -7 * 60 };     //Standard time = UTC - 7 hours
Timezone tz(myDST, mySTD);

} // namespace

void get_time_begin()
{
    ntp.begin();
}


String get_date_time_string()
{
    time_t now = tz.toLocal(ntp.get_time());

    char time_string[256];
    snprintf(time_string, sizeof(time_string), "%u:%02u:%02u on %u/%u/%u", hour(now), minute(now), second(now), month(now), day(now), year(now));

    return String(time_string);
}
