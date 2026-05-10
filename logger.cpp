#include <SD_MMC.h>

#include "logger.h"
#include "config.h"
#include "rtc.h"

// ===== НАСТРОЙКИ =====
#define LOG_TO_SERIAL false
#define LOG_TO_SD SD_ENABLE

static File logFile;

static bool rtcReady = false;

void loggerSetRTCReady(bool ready) {
  rtcReady = ready;
}

// ===== ВСПОМОГАТЕЛЬНО =====
static String getTimestamp() {
  if (!rtcReady) {
    return "[boot]";
  }

  DateTime now = getNow();

  if (now.year() < 2023) {
    return "[no-time]";
  }

  char buf[25];
  sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
          now.year(), now.month(), now.day(),
          now.hour(), now.minute(), now.second());

  return String(buf);
}

static void writeLog(const String& level, const String& msg) {

  String line = getTimestamp() + " [" + level + "] " + msg;

#if LOG_TO_SERIAL
  Serial.println(line);
#endif

#if LOG_TO_SD
  if (SD_MMC.begin("/sdcard", true)) {
    logFile = SD_MMC.open("/log.txt", FILE_APPEND);

    if (logFile) {
      logFile.println(line);
      logFile.close();
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