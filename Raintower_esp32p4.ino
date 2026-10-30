// =====================================================================
//  RainTower — контроллер аэропонной установки
//  Плата: ESP32-P4-Module (ESP32-P4 + ESP32-C6 для Wi-Fi)
// =====================================================================
#include "config.h"
#include "actuators.h"
#include "ds18b20.h"
#include "lcd.h"
#include "logger.h"
#include "mqtt.h"
#include "network_local.h"
#include "ota.h"
#include "rtc.h"
#include "SD.h"
#include "sensors.h"
#include "telemetry.h"
#include "watchdog.h"

unsigned long now_millis, lastMsg, lastResetTime = 0;
const unsigned long resetInterval = 86400000;
unsigned long lastLightCheck = 0;
// const unsigned long rtcRetryInterval = 5UL * 60UL * 1000UL;  // 5 minutes

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  if (SD_ENABLE) {
    if (SD_Init()) {
      logger_Init();
    }
  }
  watchdog_Init();
  lcd_Init();
  Serial.println("Система загружается");
  logInfo("Система загружается");
  lcd.setCursor(0, 0);
  lcd.print("      RAINTOWER");
  if (ETH_ENABLE) {
    ethernet_Init();
  }
  lcd.setCursor(0, 1);
  lcd.print("Ethernet started");
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  waitForWifi();
  lcd.setCursor(0, 2);
  lcd.print("Wifi Started");
  rtc_Init();
  set_Rtc();
  loggerSetRTCReady(true);
  ds18b20_Init();
  sensors_Init();
  actuators_Init();
  mqtt_Init();
  OTA_Init();
  lcd.setCursor(0, 3);
  lcd.print("System ready");
  Serial.println("Система готова");
  logInfo("Система готова");
  logResetReason();
  Serial.printf("Wi-Fi status после waitForWifi(): %d\n",(int)WiFi.status());
  Serial.printf("Wi-Fi IP: %s\n", WiFi.localIP().toString().c_str());
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
  actuators_Loop(now);

  if (now_millis - lastMsg > SENDING_INTERVAL_MINUTES * 60000) {
    lastMsg = now_millis;

    static int previousDay = -1;
    if (now.day() != previousDay) {
      previousDay = now.day();
      lastSyncedDay = -1;
    }

    if ((now.dayOfTheWeek() == 0 ||
         now.dayOfTheWeek() == 4) &&
        lastSyncedDay != now.day()) {

      Serial.println("Cинхронизация времени");
      logInfo("Синхронизация времени");
      syncRTCwithNTP();
      lastSyncedDay = now.day();
    }
  }
  watchdog_Reset();
}