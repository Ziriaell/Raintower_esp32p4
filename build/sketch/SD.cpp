#line 1 "C:\\Users\\User\\Documents\\Repo\\Raintower_esp32p4\\SD.cpp"
#include "FS.h"
#include "SD_MMC.h"
#include "SD.h"
#include "logger.h"
#include "config.h"

bool SD_Init() {
  Serial.println("Инициализация SD-карты");
  // logInfo("Инициализация SD-карты");

  // Включаем питание карты
  pinMode(SD_POWER_PIN, OUTPUT);
  digitalWrite(SD_POWER_PIN, HIGH);
  delay(100);

  if (!SD_MMC.setPins(
        SDMMC_CLK,
        SDMMC_CMD,
        SDMMC_D0,
        SDMMC_D1,
        SDMMC_D2,
        SDMMC_D3)) {
    Serial.println("Конфигурация контактов SD-карты не верная!");
    logError("Конфигурация контактов SD-карты не верная!");
    return false;
  }

  if (!SD_MMC.begin("/sdcard", true)) {  // true = 1-bit mode fallback
    Serial.println("Инициализация SD-карты не удалась!");
    logError("Инициализация SD-карты не удалась!");
    return false;
  }

  Serial.println("SD-карта инициализирована");
  logInfo("SD-карта инициализирована");

  return true;
}

// void logToSD(const char* tag, String message) {
//   File file = SD_MMC.open("/log.txt", FILE_APPEND);
//   if (!file) return;

//   file.printf("[%lu][%s] %s\n", millis(), tag, message.c_str());
//   file.close();
// }
