#line 1 "C:\\Users\\User\\Documents\\Repo\\Raintower_esp32p4\\rtc.h"
#ifndef RTC_H
#define RTC_H

#include <RTClib.h>

extern bool rtcAvailable;
extern unsigned long lastNtpMillis;
extern DateTime lastNtpTime;

void rtc_Init();
void set_Rtc();
void syncRTCwithNTP();
DateTime getNow();

#endif