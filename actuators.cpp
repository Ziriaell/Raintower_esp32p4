#include <iarduino_I2C_Relay.h>

#include "actuators.h"
#include "rtc.h"
#include "SD.h"
#include "logger.h"
#include "config.h"
#include "mqtt.h"

int supplementMinutes = 0;
int lastSyncedDay = -1;
bool lightOn = false;

int sunriseMinutesToday = 0;
int sunsetMinutesToday = 0;
int morningSupplementMinutes = 0;
int eveningSupplementMinutes = 0;

SunSet sun;
iarduino_I2C_Relay POWER_KEYS(POWER_KEYS_ADDRESS);
iarduino_I2C_Relay RELAY(RELAY_ADDRESS);

void actuators_Init() {
  RELAY.begin();
  POWER_KEYS.begin();
  RELAY.digitalWrite(ALL_CHANNEL, LOW);
  POWER_KEYS.digitalWrite(ALL_CHANNEL, LOW);
  sun.setPosition(LATITUDE, LONGITUDE, TIMEZONE);
  Serial.println("Инициализация системы управления исполнительными устройствами окончена");
  logInfo("Инициализация системы управления исполнительными устройствами окончена");
}

void actuators_Loop(DateTime now) {
  // Расчёт расписания при старте и при смене даты
  static int lastYear = -1;
  static int lastMonth = -1;
  static int lastDay = -1;

  if (now.year() != lastYear || now.month() != lastMonth || now.day() != lastDay) {

    lastYear = now.year();
    lastMonth = now.month();
    lastDay = now.day();

    calculateLightingNeeds(now);
  }

  // Периодическая проверка досветки
  static unsigned long lastLightCheck = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastLightCheck >= LIGHT_CHECK_INTERVAL) {
    lastLightCheck = currentMillis;
    controlLighting(now);
  }
}

void SetPowerKey(uint8_t channel, bool state) {
  if (channel < 1 || channel > 4)
    return;

  POWER_KEYS.digitalWrite(channel, state);
}

void SetRelay(uint8_t channel, bool state) {
  if (channel < 1 || channel > 2)
    return;

  RELAY.digitalWrite(channel, state);
}

bool GetRelay(uint8_t channel) {
  return RELAY.digitalRead(channel);
}

bool GetPowerKey(uint8_t channel) {
  return POWER_KEYS.digitalRead(channel);
}

// void calculateLightingNeeds(DateTime now) {
//   sun.setCurrentDate(now.year(), now.month(), now.day());

//   float sunrise = sun.calcSunrise();
//   float sunset = sun.calcSunset();

//   int sunriseMinutes = (int)sunrise;
//   int sunsetMinutes = (int)sunset;
//   int dayLength = sunsetMinutes - sunriseMinutes;

//   supplementMinutes = REQUIRED_LIGHT_MINUTES - dayLength;
//   if (supplementMinutes < 0) supplementMinutes = 0;

//   Serial.println("========== LIGHTING CALCULATION ==========");
//   Serial.printf("Date: %04d-%02d-%02d\n",
//                 now.year(), now.month(), now.day());

//   Serial.printf("Sunrise raw: %.4f hours\n", sunrise);
//   Serial.printf("Sunset raw:  %.4f hours\n", sunset);

//   Serial.printf("Sunrise: %02d:%02d (%d min)\n",
//                 sunriseMinutes / 60,
//                 sunriseMinutes % 60,
//                 sunriseMinutes);

//   Serial.printf("Sunset:  %02d:%02d (%d min)\n",
//                 sunsetMinutes / 60,
//                 sunsetMinutes % 60,
//                 sunsetMinutes);

//   Serial.printf("Day length: %d min (%d h %d min)\n",
//                 dayLength,
//                 dayLength / 60,
//                 dayLength % 60);

//   Serial.printf("Required light: %d min\n", REQUIRED_LIGHT_MINUTES);
//   Serial.printf("Supplement: %d min (%d h %d min)\n",
//                 supplementMinutes,
//                 supplementMinutes / 60,
//                 supplementMinutes % 60);

//   Serial.println("===========================================");
// }

// void controlLighting(DateTime now) {

//   int currentMinutes = now.hour() * 60 + now.minute();
//   int sunsetToday = (int)sun.calcSunset();

//   bool shouldLight =
//     supplementMinutes > 0 && currentMinutes >= sunsetToday && currentMinutes < (sunsetToday + supplementMinutes);

//   // Диагностика не чаще одного раза в минуту
//   static unsigned long lastLightDiagnostic = 0;

//   if (millis() - lastLightDiagnostic >= 60000UL) {
//     lastLightDiagnostic = millis();

//     Serial.println("------------- LIGHTING CHECK -------------");
//     Serial.printf("Current time: %02d:%02d:%02d (%d min)\n",
//                   now.hour(), now.minute(), now.second(),
//                   currentMinutes);

//     Serial.printf("Sunset: %02d:%02d (%d min)\n",
//                   sunsetToday / 60,
//                   sunsetToday % 60,
//                   sunsetToday);

//     Serial.printf("Supplement: %d min\n", supplementMinutes);
//     Serial.printf("Light interval: %d - %d min\n",
//                   sunsetToday,
//                   sunsetToday + supplementMinutes);

//     Serial.printf("Should light: %s\n",
//                   shouldLight ? "YES" : "NO");
//     Serial.printf("Current lightOn state: %s\n",
//                   lightOn ? "ON" : "OFF");

//     Serial.println("------------------------------------------");
//   }

//   if (shouldLight) {
//     if (!lightOn) {
//       SetPowerKey(LIGHT_PIN_CHANNEL, HIGH);
//       lightOn = true;

//       Serial.println("Досветка включена");
//       logInfo("Досветка включена");
//       mqtt_client.publish("home/aeroponic/switch/led1/state", "ON", true);
//     }
//   } else {
//     if (lightOn) {
//       SetPowerKey(LIGHT_PIN_CHANNEL, LOW);
//       lightOn = false;

//       Serial.println("Досветка выключена");
//       logInfo("Досветка выключена");
//       mqtt_client.publish("home/aeroponic/switch/led1/state", "OFF", true);
//     }
//   }
// }


void calculateLightingNeeds(DateTime now) {
  sun.setCurrentDate(now.year(), now.month(), now.day());

  float sunrise = sun.calcSunrise();
  float sunset = sun.calcSunset();

  sunriseMinutesToday = (int)sunrise;
  sunsetMinutesToday = (int)sunset;

  int dayLength = sunsetMinutesToday - sunriseMinutesToday;

  supplementMinutes = REQUIRED_LIGHT_MINUTES - dayLength;
  if (supplementMinutes < 0) {
    supplementMinutes = 0;
  }

  // Делим дополнительное освещение между утром и вечером.
  morningSupplementMinutes = supplementMinutes / 2;
  eveningSupplementMinutes =
      supplementMinutes - morningSupplementMinutes;

  Serial.println("========== LIGHTING CALCULATION ==========");
  Serial.printf("Date: %04d-%02d-%02d\n",
                now.year(), now.month(), now.day());

  Serial.printf("Sunrise: %02d:%02d (%d min)\n",
                sunriseMinutesToday / 60,
                sunriseMinutesToday % 60,
                sunriseMinutesToday);

  Serial.printf("Sunset:  %02d:%02d (%d min)\n",
                sunsetMinutesToday / 60,
                sunsetMinutesToday % 60,
                sunsetMinutesToday);

  Serial.printf("Day length: %d min (%d h %d min)\n",
                dayLength, dayLength / 60, dayLength % 60);

  Serial.printf("Required light: %d min\n",
                REQUIRED_LIGHT_MINUTES);

  Serial.printf("Total supplement: %d min\n",
                supplementMinutes);

  Serial.printf("Morning supplement: %d min\n",
                morningSupplementMinutes);

  Serial.printf("Evening supplement: %d min\n",
                eveningSupplementMinutes);

  Serial.printf("Morning window: %02d:%02d - %02d:%02d\n",
                (sunriseMinutesToday - morningSupplementMinutes) / 60,
                (sunriseMinutesToday - morningSupplementMinutes) % 60,
                sunriseMinutesToday / 60,
                sunriseMinutesToday % 60);

  Serial.printf("Evening window: %02d:%02d - %02d:%02d\n",
                sunsetMinutesToday / 60,
                sunsetMinutesToday % 60,
                (sunsetMinutesToday + eveningSupplementMinutes) / 60,
                (sunsetMinutesToday + eveningSupplementMinutes) % 60);

  Serial.println("===========================================");
}


void controlLighting(DateTime now) {
  int currentMinutes = now.hour() * 60 + now.minute();

  int morningStart =
      sunriseMinutesToday - morningSupplementMinutes;
  int morningEnd = sunriseMinutesToday;

  int eveningStart = sunsetMinutesToday;
  int eveningEnd =
      sunsetMinutesToday + eveningSupplementMinutes;

  bool morningWindow =
      morningSupplementMinutes > 0 &&
      currentMinutes >= morningStart &&
      currentMinutes < morningEnd;

  bool eveningWindow =
      eveningSupplementMinutes > 0 &&
      currentMinutes >= eveningStart &&
      currentMinutes < eveningEnd;

  bool shouldLight = morningWindow || eveningWindow;

  // Диагностика не чаще одного раза в минуту
  static unsigned long lastLightDiagnostic = 0;

  if (millis() - lastLightDiagnostic >= 60000UL) {
    lastLightDiagnostic = millis();

    Serial.println("------------- LIGHTING CHECK -------------");

    Serial.printf("Current time: %02d:%02d:%02d (%d min)\n",
                  now.hour(), now.minute(), now.second(),
                  currentMinutes);

    Serial.printf("Morning window: %d - %d min\n",
                  morningStart, morningEnd);

    Serial.printf("Evening window: %d - %d min\n",
                  eveningStart, eveningEnd);

    Serial.printf("Morning active: %s\n",
                  morningWindow ? "YES" : "NO");

    Serial.printf("Evening active: %s\n",
                  eveningWindow ? "YES" : "NO");

    Serial.printf("Should light: %s\n",
                  shouldLight ? "YES" : "NO");

    Serial.printf("Current lightOn state: %s\n",
                  lightOn ? "ON" : "OFF");

    Serial.println("------------------------------------------");
  }

  if (shouldLight) {
    if (!lightOn) {
      SetPowerKey(LIGHT_PIN_CHANNEL, HIGH);
      lightOn = true;

      Serial.println("Досветка включена");
      logInfo("Досветка включена");
      mqtt_client.publish(
          "home/aeroponic/switch/led1/state", "ON", true);
    }
  } else {
    if (lightOn) {
      SetPowerKey(LIGHT_PIN_CHANNEL, LOW);
      lightOn = false;

      Serial.println("Досветка выключена");
      logInfo("Досветка выключена");
      mqtt_client.publish(
          "home/aeroponic/switch/led1/state", "OFF", true);
    }
  }
}

