#include "ds18b20.h"
#include "logger.h"
#include "config.h"

#include "OneWireNg_CurrentPlatform.h"
#include "drivers/DSTherm.h"
#include "utils/Placeholder.h"

// ===== НАСТРОЙКИ =====
#define OW_PIN TEMP_SENSOR_PIN
#define PARASITE_POWER_ARG false

// ===== ONE WIRE =====
static Placeholder<OneWireNg_CurrentPlatform> ow;
static bool initialized = false;

// ===== INIT =====
void ds18b20_Init()
{
#ifdef PWR_CTRL_PIN
    new (&ow) OneWireNg_CurrentPlatform(OW_PIN, PWR_CTRL_PIN, false);
#else
    new (&ow) OneWireNg_CurrentPlatform(OW_PIN, false);
#endif

    initialized = true;

    Serial.println("DS18B20 инициализован");
    logInfo("DS18B20 инициализован");

    
}

// ===== ЧТЕНИЕ ТЕМПЕРАТУРЫ =====
float getWaterTemp()
{
    if (!initialized) return NAN;

    DSTherm drv(ow);

    // запускаем измерение
    drv.convertTempAll(DSTherm::MAX_CONV_TIME, PARASITE_POWER_ARG);

    // используем single sensor режим (как у тебя)
    static PlaceholderInit<DSTherm::Scratchpad> scrpd;

    OneWireNg::ErrorCode ec = drv.readScratchpadSingle(scrpd);

    if (ec == OneWireNg::EC_SUCCESS) {
        long temp = scrpd->getTemp2();
        return (float)temp / 16.0;
    }

    if (ec == OneWireNg::EC_CRC_ERROR) {
        Serial.println("DS18B20 ошибка CRC ");
        logError("DS18B20 ошибка CRC ");
    }

    return NAN;
}
