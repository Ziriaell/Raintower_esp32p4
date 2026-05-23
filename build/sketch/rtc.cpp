#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\rtc.cpp"
#include <NTPClient.h>

#include "rtc.h"
#include "network_local.h"
#include "logger.h"
#include "config.h"

bool rtcAvailable = true;
static bool timeValid = false;

unsigned long lastNtpMillis = 0;


DateTime lastNtpTime;
static NetworkUDP ntpUDP;
static RTC_DS3231 rtc;
static NTPClient timeClient(ntpUDP, "pool.ntp.org", TIMEZONE * 3600);


void rtc_Init() {
  if (!rtc.begin()) {
    Serial.println("RTC не доступно, используем NTP.");
    logInfo("RTC не доступно, используем NTP.");
    rtcAvailable = false;
  } else {
    timeValid = true;
    loggerSetRTCReady(true);
    if (rtc.lostPower()) {
      Serial.println("RTC было некорректно выключено. Синхронизация времени.");
      logError("RTC было некорректно выключено. Синхронизация времени.");
      syncRTCwithNTP();
    }
  }
}

bool isTimeValid() {
  return timeValid;
}

void syncRTCwithNTP() {
  if (isNetworkReady()) {
    timeClient.begin();
    if (timeClient.forceUpdate()) {
      unsigned long epochTime = timeClient.getEpochTime();
      lastNtpTime = DateTime(epochTime);
      lastNtpMillis = millis();
      if (rtcAvailable) {
        rtc.adjust(DateTime(epochTime));
      }
      timeValid = true;
      loggerSetRTCReady(true);
      Serial.println("Часы синхронизированны с NTP");
      logInfo("Часы синхронизированны с NTP");
    } else {
      Serial.println("Не удалось синхронизироваться");
      logInfo("Не удалось синхронизироваться");
    }
  }
}

void set_Rtc() {
  if (!rtcAvailable) {
    // Initialize NTP and get initial time
    if (isNetworkReady()) {
      timeClient.begin();
      if (timeClient.forceUpdate()) {
        lastNtpTime = DateTime(timeClient.getEpochTime());
        lastNtpMillis = millis();
        Serial.println("Использование NTP в качестве резервных часов.");
        logInfo("Использование NTP в качестве резервных часов.");
        timeValid = true;
        loggerSetRTCReady(true);
      } else {
        Serial.println("Не удалось синхронизироваться с NTP.");
        logInfo("Не удалось синхронизироваться с NTP.");
      }
    }
  }

  if (rtcAvailable) {
    DateTime now = getNow();
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

  if (rtcAvailable) {
    return rtc.now();
  }

  unsigned long elapsed =
    (millis() - lastNtpMillis) / 1000;

  return lastNtpTime + TimeSpan(elapsed);
}