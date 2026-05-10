#include <NTPClient.h>

#include "rtc.h"
#include "network_local.h"
#include "logger.h"
#include "config.h"

bool rtcAvailable = true;
unsigned long lastNtpMillis = 0;

DateTime lastNtpTime;
static WiFiUDP ntpUDP;
static RTC_DS3231 rtc;
static NTPClient timeClient(ntpUDP, "pool.ntp.org", TIMEZONE * 3600);


void rtc_Init() {
  if (!rtc.begin()) {
    Serial.println("RTC не доступно, используем NTP.");
    logInfo("RTC не доступно, используем NTP.");
    rtcAvailable = false;
  } else {
    if (rtc.lostPower()) {
      Serial.println("RTC было некорректно выключено. Синхронизация времени.");
      logError("RTC было некорректно выключено. Синхронизация времени.");
      syncRTCwithNTP();
    }
  }
}

void syncRTCwithNTP() {
  if (check_Wifi()) {
    timeClient.begin();
    if (timeClient.forceUpdate()) {
      unsigned long epochTime = timeClient.getEpochTime();
      rtc.adjust(DateTime(epochTime));
      Serial.println("Часы синхронизированны с NTP");
      logInfo("Часы синхронизированны с NTP");
    } else {
      Serial.println("Не удалось синхронизироваться");
      logInfo("Не удалось синхронизироваться");
    }
  }
}

void set_Rtc(){
    if (!rtcAvailable) {
    // Initialize NTP and get initial time
    if (check_Wifi()) {
      timeClient.begin();
      if (timeClient.forceUpdate()) {
        lastNtpTime = DateTime(timeClient.getEpochTime());
        lastNtpMillis = millis();
        Serial.println("Использование NTP в качестве резервных часов.");
        logInfo("Использование NTP в качестве резервных часов.");
      } else {
        Serial.println("Не удалось синхронизироваться с NTP.");
        logInfo("Не удалось синхронизироваться с NTP.");
      }
    }
  }

  if (rtcAvailable) {
    DateTime now = rtc.now();
    syncRTCwithNTP();
    Serial.printf("Текущее время RTC: %04d-%02d-%02d %02d:%02d:%02d\n",
                  now.year(), now.month(), now.day(),
                  now.hour(), now.minute(), now.second());
  } else {
    Serial.printf("Текущее время NTP: %04d-%02d-%02d %02d:%02d:%02d\n",
                  lastNtpTime.year(), lastNtpTime.month(), lastNtpTime.day(),
                  lastNtpTime.hour(), lastNtpTime.minute(), lastNtpTime.second());
  }
}
DateTime getNow() {
  return rtc.now();
}