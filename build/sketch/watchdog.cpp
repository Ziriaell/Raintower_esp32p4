#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\watchdog.cpp"
#include <esp_task_wdt.h>
#include <esp_system.h>

#include "watchdog.h"
#include "logger.h"

void watchdog_Init() {

    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 30000,
        .idle_core_mask = 0,
        .trigger_panic = true
    };

    esp_task_wdt_init(&wdt_config);

    esp_task_wdt_add(NULL);

    logInfo("Watchdog initialized");
}

void watchdog_Reset() {
    esp_task_wdt_reset();
}

void logResetReason() {

    esp_reset_reason_t reason =
        esp_reset_reason();

    switch (reason) {

        case ESP_RST_POWERON:
            logInfo("Reset reason: POWERON");
            break;

        case ESP_RST_EXT:
            logInfo("Reset reason: EXTERNAL");
            break;

        case ESP_RST_SW:
            logInfo("Reset reason: SOFTWARE");
            break;

        case ESP_RST_PANIC:
            logError("Reset reason: PANIC");
            break;

        case ESP_RST_INT_WDT:
            logError("Reset reason: INT_WDT");
            break;

        case ESP_RST_TASK_WDT:
            logError("Reset reason: TASK_WDT");
            break;

        case ESP_RST_WDT:
            logError("Reset reason: OTHER_WDT");
            break;

        case ESP_RST_BROWNOUT:
            logError("Reset reason: BROWNOUT");
            break;

        case ESP_RST_SDIO:
            logError("Reset reason: SDIO");
            break;

        default:
            logError("Reset reason: UNKNOWN");
            break;
    }
}