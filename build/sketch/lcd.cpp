#line 1 "C:\\Users\\User\\Documents\\Repo\\Raintower_esp32p4\\lcd.cpp"
#include <LiquidCrystal_PCF8574.h>

#include "lcd.h"
#include "rtc.h"
#include "mqtt.h"
#include "network.h"
#include "sensors.h"
#include "logger.h"

LiquidCrystal_PCF8574 lcd(0x27);

bool lcdAvailable = false;

static unsigned long lastLcdUpdate = 0;

const uint32_t LCD_UPDATE_INTERVAL = 1000; // 1 секунда

// ===== ДОПОЛНЕНИЕ СТРОКИ ПРОБЕЛАМИ =====
static String padLine(String text) {

    while (text.length() < 20) {
        text += " ";
    }

    return text.substring(0, 20);
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

    // Тестовый вывод
    lcd.setCursor(0, 0);
    lcd.print("RainTower");

    lcd.setCursor(0, 1);
    lcd.print("System boot...");

    lcd.setCursor(0, 2);
    lcd.print("LCD2004 OK");

    lcd.setCursor(0, 3);
    lcd.print("Please wait");

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

    // ===== PH + EC =====
    char line2[21];

    sprintf(line2,
            "PH:%1.2f EC:%1.2f",
            PH,
            EC);

    lcd.setCursor(0, 1);
    lcd.print(padLine(line2));

    // ===== TDS + WATER TEMP =====
    char line3[21];

    sprintf(line3,
            "TDS:%3.0f WT:%2.1f",
            TDS,
            Water_temperature);

    lcd.setCursor(0, 2);
    lcd.print(padLine(line3));

    // ===== MQTT + NETWORK =====
    char line4[21];

    const char* mqttState =
        mqtt_client.connected() ? "MQTT:OK" : "MQTT:OFF";

    const char* netState =
        isEthConnected() ? "ETH" :
        isWifiConnected() ? "WIFI" :
        "OFF";

    sprintf(line4,
            "%s %s",
            mqttState,
            netState);

    lcd.setCursor(0, 3);
    lcd.print(padLine(line4));
}