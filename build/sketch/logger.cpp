#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\logger.cpp"
#include <SD_MMC.h>

#include "logger.h"
#include "config.h"
#include "rtc.h"

// ===== НАСТРОЙКИ =====

#define LOG_TO_SERIAL false
#define LOG_TO_SD SD_ENABLE

static File logFile;

static bool rtcReady = false;
static bool logReady = false;

// ===== INIT =====

bool logger_Init() {

#if LOG_TO_SD

  logFile = SD_MMC.open("/log.txt", FILE_APPEND);

  if (!logFile) {

    Serial.println("Не удалось открыть log.txt");

    return false;
  }

  logReady = true;

#endif

  return true;
}

// ===== CLOSE =====

void logger_Close() {

#if LOG_TO_SD

  if (logFile) {

    logFile.flush();
    logFile.close();
  }

#endif
}

// ===== RTC STATE =====

void loggerSetRTCReady(bool ready) {

  rtcReady = ready;
}

// ===== TIMESTAMP =====

static String getTimestamp() {

  if (!rtcReady) {
    return "[boot]";
  }

  DateTime now = getNow();

  if (now.year() < 2023) {
    return "[no-time]";
  }

  char buf[25];

  sprintf(buf,
          "%04d-%02d-%02d %02d:%02d:%02d",
          now.year(),
          now.month(),
          now.day(),
          now.hour(),
          now.minute(),
          now.second());

  return String(buf);
}

// ===== WRITE =====

static void writeLog(const String& level,
                     const String& msg) {

  String line =
    getTimestamp() + " [" + level + "] " + msg;

#if LOG_TO_SERIAL

  Serial.println(line);

#endif

#if LOG_TO_SD

  if (logReady && logFile) {

    if (logFile.println(line)) {

      logFile.flush();

    } else {

      Serial.println("Ошибка записи в log.txt");
    }
  }

#endif
}

// ===== API =====

void logInfo(const String& msg) {

  writeLog("INFO", msg);
}

void logError(const String& msg) {

  writeLog("ERROR", msg);
}

void logDebug(const String& msg) {

  writeLog("DEBUG", msg);
}