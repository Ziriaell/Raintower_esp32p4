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

unsigned long now_millis, lastMsg, lastResetTime = 0;
const unsigned long resetInterval = 86400000;
unsigned long lastLightCheck = 0;
// const unsigned long rtcRetryInterval = 5UL * 60UL * 1000UL;  // 5 minutes

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
  network_Loop();
  mqtt_Loop();
  sensors_Loop();
  OTA_Loop();
  lcd_Loop();
  now_millis = millis();
  DateTime now;

  if (rtcAvailable) {
    now = getNow();
  } else {
    // estimate current time by adding millis() difference to lastNtpTime
    unsigned long secondsSinceSync = (millis() - lastNtpMillis) / 1000;
    now = lastNtpTime + TimeSpan(secondsSinceSync);
  }

  // if (millis() - lastResetTime >= resetInterval) {
  //   ESP.restart();
  // }
  if (now_millis - lastMsg > SENDING_INTERVAL_MINUTES * 60000) {
    lastMsg = now_millis;
    // Указываем текущую температуру жидкости.
    // Serial.print(EC);              // Выводим удельную электропроводность жидкости приведённую к опорной температуре.
    // Serial.print("мСм/см, TDS=");  //
    // Serial.print(TDS);             // Выводим количество растворённых твёрдых веществ в жидкости.
    // Serial.print(" мг/л\r\n");

    char buf[50];
    char tempString[8];
    dtostrf(Water_temperature, 1, 2, tempString);
    Serial.print("Температура воды ");
    Serial.print(tempString);
    Serial.println("°C");
    sprintf(buf, "Температура воды %s ", tempString);
    logInfo(buf);
    mqtt_client.publish("home/aeroponic/sensors/water_temperature", tempString, true);

    if (PH > 0 && PH <= 14) {
      char pHString[8];
      dtostrf(PH, 1, 2, pHString);
      Serial.print("Кислотность ");  //
      Serial.print(pHString);        // Выводим водородный показатель жидкости с 1 знаком после запятой.
      Serial.println(" pH.\r\n");
      sprintf(buf, "pH %s ед", pHString);
      logInfo(buf);
      mqtt_client.publish("home/aeroponic/sensors/ph", pHString, true);
    }
    char ECString[8];
    dtostrf(EC, 1, 2, ECString);
    Serial.print("EC ");
    Serial.print(ECString);
    Serial.println("мСм/см");
    sprintf(buf, "EC %s мСм/см", ECString);
    logInfo(buf);
    mqtt_client.publish("home/aeroponic/sensors/ec", ECString, true);

    char TDSString[8];
    dtostrf(TDS, 1, 2, TDSString);
    Serial.print("TDS ");
    Serial.print(TDSString);
    Serial.println(" мг/л\r\n");
    sprintf(buf, "TDS %s мг/л", TDSString);
    logInfo(buf);
    mqtt_client.publish("home/aeroponic/sensors/tds", TDSString, true);

    if (!isnan(Air_temperature) || !isnan(Air_humidity)) {
      char Air_temperatureString[8];
      dtostrf(Air_temperature, 1, 2, Air_temperatureString);
      Serial.print("Температура воздуха ");
      Serial.print(Air_temperatureString);
      Serial.println(" °C");
      mqtt_client.publish("home/balcony/sensors/temperature", Air_temperatureString, true);

      char Air_humidityString[8];
      dtostrf(Air_humidity, 1, 2, Air_humidityString);
      Serial.print("Влажность воздуха ");
      Serial.print(Air_humidityString);
      Serial.println(" %");
      mqtt_client.publish("home/balcony/sensors/humidity", Air_humidityString, true);
    }

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