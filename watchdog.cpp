#include <esp_task_wdt.h>
#include <esp_system.h>
#include <esp_err.h>

#include "watchdog.h"
#include "logger.h"

void watchdog_Init() {

    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 30000,
        .idle_core_mask = 0,
        .trigger_panic = true
    };

    // Сначала пытаемся инициализировать watchdog.
    esp_err_t err = esp_task_wdt_init(&wdt_config);

    // Если Arduino Core уже инициализировал TWDT,
    // изменяем настройки существующего watchdog.
    if (err == ESP_ERR_INVALID_STATE) {
        err = esp_task_wdt_reconfigure(&wdt_config);
    }

    if (err != ESP_OK) {
        logError("Watchdog configuration failed");
        return;
    }

    // Регистрируем текущую задачу (loopTask).
    err = esp_task_wdt_add(NULL);

    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        logError("Watchdog task registration failed");
        return;
    }

    logInfo("Watchdog initialized: 30 seconds");
}

void watchdog_Reset() {
    esp_task_wdt_reset();
}

void logResetReason() {

    esp_reset_reason_t reason = esp_reset_reason();

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