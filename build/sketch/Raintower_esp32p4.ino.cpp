#include <Arduino.h>
#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\Raintower_esp32p4.ino"
#include "config.h"
#include "network_local.h"
#include "mqtt.h"
#include "sensors.h"
#include "ds18b20.h"
#include "rtc.h"
#include "lighting.h"
#include "SD.h"
#include "logger.h"
#include "ota.h"
#include "lcd.h"
#include "telemetry.h"

unsigned long now_millis, lastMsg, lastResetTime = 0;
const unsigned long resetInterval = 86400000;
unsigned long lastLightCheck = 0;
// const unsigned long rtcRetryInterval = 5UL * 60UL * 1000UL;  // 5 minutes

#line 19 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\Raintower_esp32p4.ino"
void setup();
#line 61 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\Raintower_esp32p4.ino"
void loop();
#line 19 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\Raintower_esp32p4.ino"
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(500);
  }
  if (SD_ENABLE) {
    if (SD_Init()) {
      logger_Init();
    }
  }
  lcd_Init();
  Serial.println("Система загружается");
  logInfo("Система загружается");
  lcd.setCursor(0, 0);
  lcd.print("      RAINTOWER");
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  waitForWifi();
  lcd.setCursor(0, 1);
  lcd.print("Wifi Started");
  rtc_Init();
  set_Rtc();
  loggerSetRTCReady(true);
  if (ETH_ENABLE) {
    ethernet_Init();
  }
  lcd.setCursor(0, 2);
  lcd.print("Ethernet started");
  ds18b20_Init();
  sensors_Init();
  lighting_Init();
  mqtt_Init();
  OTA_Init();
  lcd.setCursor(0, 3);
  lcd.print("System ready");
  Serial.println("Система готова");
  logInfo("Система готова");
  Serial.println("============================================");
}

void loop() {
  now_millis = millis();
  DateTime now = getNow();
  network_Loop();
  mqtt_Loop();
  sensors_Loop();
  OTA_Loop();
  lcd_Loop();
  telemetry_Loop();


  // if (millis() - lastResetTime >= resetInterval) {
  //   ESP.restart();
  // }
  if (now_millis - lastMsg > SENDING_INTERVAL_MINUTES * 60000) {
    lastMsg = now_millis;

    // publishFloat("home/aeroponic/sensors/water_temperature", Water_temperature)

    // Reset sync flag daily
    static int previousDay = -1;
    if (now.day() != previousDay) {
      previousDay = now.day();
      lastSyncedDay = -1;
    }
    // Weekly NTP sync on Sunday
    if (now.dayOfTheWeek() == 0 && lastSyncedDay != now.day()) {
      Serial.println("📅 Sunday sync");
      logInfo("Воскресная синхронизация времени");
      syncRTCwithNTP();
      lastSyncedDay = now.day();
    }
    // Daily sunrise/sunset calculations
    static int lastDay = -1;
    if (now.day() != lastDay) {
      lastDay = now.day();
      calculateLightingNeeds(now);
    }
  }
  if (now_millis - lastLightCheck > LIGHT_CHECK_INTERVAL) {
    lastLightCheck = now_millis;
    controlLighting(now);
  }
}
