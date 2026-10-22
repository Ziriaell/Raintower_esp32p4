#include <ArduinoOTA.h>

#include "ota.h"
#include "logger.h"
#include "config.h"
#include "watchdog.h"

void OTA_Init() {

    ArduinoOTA.setHostname(OTA_HOSTNAME);

    // Пароль OTA
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([]() {

        watchdog_Reset();
        logInfo("OTA start");

    });

    ArduinoOTA.onEnd([]() {

        watchdog_Reset();
        logInfo("OTA end");

    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {

        // Обслуживаем watchdog во время передачи прошивки.
        watchdog_Reset();

        if (total == 0) {
            return;
        }

        char buf[64];

        snprintf(buf, sizeof(buf),
                 "OTA Progress: %u%%",
                 (progress * 100U) / total);

        Serial.println(buf);
        // logInfo(buf);

    });

    ArduinoOTA.onError([](ota_error_t error) {

        char buf[64];

        snprintf(buf, sizeof(buf),
                 "OTA Error[%u]",
                 static_cast<unsigned int>(error));

        logError(buf);

    });

    ArduinoOTA.begin();

    logInfo("OTA ready");
}

void OTA_Loop() {
    ArduinoOTA.handle();
}