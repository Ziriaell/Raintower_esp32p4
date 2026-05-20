#line 1 "C:\\Users\\User\\Documents\\Repo\\Raintower_esp32p4\\lcd.cpp"
#include "lcd.h"
#include "rtc.h"
#include "mqtt.h"
#include "network_local.h"
#include "sensors.h"
#include "logger.h"
#include "config.h"

LiquidCrystal_PCF8574 lcd(LCD_ADDRESS);

bool lcdAvailable = false;

static unsigned long lastLcdUpdate = 0;

static uint8_t currentPage = 0;

static unsigned long lastPageSwitch = 0;


// ===== ДОПОЛНЕНИЕ СТРОКИ ПРОБЕЛАМИ =====
static String padLine(String text) {

  while (text.length() < 20) {
    text += " ";
  }

  return text.substring(0, 20);
}

static String getWifiRSSI() {

  if (!isWifiConnected()) {
    return "OFF";
  }

  return String(WiFi.RSSI()) + "dBm";
}

static String getNetworkState() {

  bool eth = isEthConnected();
  bool wifi = isWifiConnected();

  if (eth && wifi) {
    return "ETH/WIFI";
  }

  if (eth) {
    return "ETH";
  }

  if (wifi) {
    return "WIFI";
  }

  return "OFF";
}

static String getUptime() {

  unsigned long sec = millis() / 1000;

  uint16_t days = sec / 86400;
  sec %= 86400;

  uint8_t hours = sec / 3600;
  sec %= 3600;

  uint8_t mins = sec / 60;

  char buf[21];

  sprintf(buf,
          "%ud %uh %um",
          days,
          hours,
          mins);

  return String(buf);
}

static void first_page() {
  // ===== ВРЕМЯ =====
  DateTime now = getNow();

  char timeBuf[21];

  sprintf(timeBuf,
          "%02d:%02d:%02d  %2.1fC",
          now.hour(),
          now.minute(),
          now.second(),
          Air_temperature);

  lcd.setCursor(0, 0);
  lcd.print(padLine(timeBuf));

  // ===== ВЛАЖНОСТЬ + ТЕМПЕРАТУРА ВОДЫ =====
  char line2[21];

  sprintf(line2,
          "AH:%2.0f%% WT:%2.1fC",
          Air_humidity,
          Water_temperature);

  lcd.setCursor(0, 1);
  lcd.print(padLine(line2));

  // ===== PH + EC =====
  char line3[21];

  sprintf(line3,
          "PH:%1.2f EC:%1.2f",
          PH,
          EC);

  lcd.setCursor(0, 2);
  lcd.print(padLine(line3));

  // ===== TDS + MQTT =====
  char line4[21];

  sprintf(line4,
          "TDS:%3.0f MQTT:%s",
          TDS,
          mqtt_client.connected() ? "OK" : "OFF");

  lcd.setCursor(0, 3);
  lcd.print(padLine(line4));
}

static void second_page() {
  lcd.setCursor(0, 0);
  lcd.print(padLine(
    "NET:" + getNetworkState()));

  lcd.setCursor(0, 1);
  lcd.print(padLine(
    "WiFi:" + getWifiRSSI()));

  String wifiIp =
    isWifiConnected()
      ? WiFi.localIP().toString()
      : "OFF";

  lcd.setCursor(0, 2);
  lcd.print(padLine(
    "W:" + wifiIp));

  String ethIp =
    isEthConnected()
      ? ETH.localIP().toString()
      : "OFF";

  lcd.setCursor(0, 3);
  lcd.print(padLine(
    "E:" + ethIp));
}

static void third_page() {
  // ===== PAGE 3 =====
  // SYSTEM

  char line1[21];

  sprintf(line1,
          "Heap:%luKB",
          ESP.getFreeHeap() / 1024);

  lcd.setCursor(0, 0);
  lcd.print(padLine(line1));

  char line2[21];

  sprintf(line2,
          "PSRAM:%luMB",
          ESP.getFreePsram() / 1024 / 1024);

  lcd.setCursor(0, 1);
  lcd.print(padLine(line2));

  lcd.setCursor(0, 2);
  lcd.print(padLine(
    "Up:" + getUptime()));

  String sdState =
    SD_ENABLE ? "OK" : "OFF";

  lcd.setCursor(0, 3);
  lcd.print(padLine(
    "SD:" + sdState + " OTA:OK"));
}

void lcd_Init() {

  Serial.println("Инициализация LCD...");
  logInfo("Инициализация LCD...");

  // Инициализация LCD
  lcd.begin(20, 4);

  // Включение подсветки
  lcd.setBacklight(255);

  // Очистка экрана
  lcd.clear();

  lcdAvailable = true;

  Serial.println("LCD инициализирован");
  logInfo("LCD инициализирован");
}

void lcd_Loop() {

    // LCD не найден
    if (!lcdAvailable) {
        return;
    }

    // Обновление раз в секунду
    if (millis() - lastLcdUpdate < LCD_UPDATE_INTERVAL) {
        return;
    }

    lastLcdUpdate = millis();

    // ===== ПЕРЕКЛЮЧЕНИЕ СТРАНИЦ =====
    if (millis() - lastPageSwitch >= PAGE_SWITCH_INTERVAL) {

        currentPage++;

        if (currentPage > 2) {
            currentPage = 0;
        }

        lastPageSwitch = millis();
    }

    switch (currentPage) {

        case 0:
            first_page();
            break;

        case 1:
            second_page();
            break;

        case 2:
            third_page();
            break;
    }
}