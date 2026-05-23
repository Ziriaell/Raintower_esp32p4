#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\ota.cpp"
#include <ArduinoOTA.h>

#include "ota.h"
#include "logger.h"

void OTA_Init() {

    ArduinoOTA.setHostname("RainTower");

    // пароль OTA (необязательно)
    ArduinoOTA.setPassword("123456");

    ArduinoOTA.onStart([]() {

        logInfo("OTA start");

    });

    ArduinoOTA.onEnd([]() {

        logInfo("OTA end");

    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {

        char buf[64];

        sprintf(buf,
                "OTA Progress: %u%%",
                (progress * 100) / total);

        logInfo(buf);

    });

    ArduinoOTA.onError([](ota_error_t error) {

        char buf[64];

        sprintf(buf,
                "OTA Error[%u]",
                error);

        logError(buf);

    });

    ArduinoOTA.begin();

    logInfo("OTA ready");
}

void OTA_Loop() {
    ArduinoOTA.handle();
}