#include "lighting.h"
#include "rtc.h"
#include "SD.h"
#include "logger.h"
#include "config.h"

int supplementMinutes = 0;
int lastSyncedDay = -1;
bool lightOn = false;

SunSet sun;

void lighting_Init() {
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
  sun.setPosition(LATITUDE, LONGITUDE, TIMEZONE);
}

// static void logLightEvent(const char* state, DateTime now)
// {
//     char filename[32];
//     char line[80];

//     // создаём папку (один раз — но можно вызывать безопасно каждый раз)
//     ensureDir("/logs");

//     // имя файла: /logs/YYYY-MM-DD.log
//     snprintf(filename, sizeof(filename),
//         "/logs/%04d-%02d-%02d.log",
//         now.year(), now.month(), now.day()
//     );

//     // строка события
//     snprintf(line, sizeof(line),
//         "%02d:%02d:%02d - LIGHT %s\n",
//         now.hour(), now.minute(), now.second(),
//         state
//     );

//     appendFile(filename, line);
// }

void calculateLightingNeeds(DateTime now) {
  sun.setCurrentDate(now.year(), now.month(), now.day());
  float sunrise = sun.calcSunrise();
  float sunset = sun.calcSunset();

  int sunriseMinutes = sunrise * 60;
  int sunsetMinutes = sunset * 60;
  int dayLength = sunsetMinutes - sunriseMinutes;

  supplementMinutes = REQUIRED_LIGHT_MINUTES - dayLength;
  if (supplementMinutes < 0) supplementMinutes = 0;

  Serial.printf("%04d-%02d-%02d\n", now.year(), now.month(), now.day());
  Serial.printf("%02d:%02d | %02d:%02d | %d min | Supplement: %d min\n",
                sunriseMinutes / 60, sunriseMinutes % 60,
                sunsetMinutes / 60, sunsetMinutes % 60,
                dayLength, supplementMinutes);
}

void controlLighting(DateTime now) {

  int currentMinutes = now.hour() * 60 + now.minute();
  int sunsetToday = sun.calcSunset() * 60;

  if (supplementMinutes > 0 &&
      currentMinutes >= sunsetToday &&
      currentMinutes < (sunsetToday + supplementMinutes)) {
    if (!lightOn) {
      digitalWrite(LIGHT_PIN, HIGH);
      lightOn = true;

      Serial.println("Досветка включена");
      logInfo("Досветка включена");
      // logLightEvent("ON", now);
    }

  } else {
    if (lightOn) {
      digitalWrite(LIGHT_PIN, LOW);
      lightOn = false;

      Serial.println("Досветка выключена");
      logInfo("Досветка выключена");
    }
  }
}
