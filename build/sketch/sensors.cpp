#line 1 "C:\\Users\\User\\Documents\\Repo\\Raintower_esp32p4\\sensors.cpp"
#include <algorithm>
#include <Wire.h>
#include <iarduino_I2C_TDS.h>
#include <iarduino_I2C_pH.h>
#include <Adafruit_AHT10.h>

#include "ds18b20.h"
#include "sensors.h"
#include "logger.h"
#include "config.h"

float PH, EC, TDS;
float Water_temperature, Air_temperature, Air_humidity;

bool sensorsReady = false;

Adafruit_AHT10 aht;

iarduino_I2C_pH pH(PH_SENSOR_ADDRESS);

iarduino_I2C_TDS tds(TDS_SENSOR_ADDRESS);

// ===== MEDIAN FILTER =====

class MedianFilter {

private:

  float buffer[NUM_READ];

  int count;

public:

  MedianFilter() {

    for (int i = 0; i < NUM_READ; i++) {
      buffer[i] = 0.0;
    }

    count = 0;
  }

  float update(float newVal) {

    buffer[count] = newVal;

    count = (count + 1) % NUM_READ;

    float sorted[NUM_READ];

    memcpy(sorted, buffer, sizeof(buffer));

    std::sort(sorted, sorted + NUM_READ);

    if (NUM_READ % 2 == 0) {

      return (sorted[NUM_READ / 2 - 1] +
              sorted[NUM_READ / 2]) / 2.0;

    } else {

      return sorted[NUM_READ / 2];
    }
  }
};

MedianFilter medianPH;
MedianFilter medianEC;
MedianFilter medianTDS;

// ===== INITIALIZATION STATE =====

static uint16_t initCounter = 0;

static unsigned long lastInitRead = 0;

const uint16_t INIT_READS = NUM_READ * 2;

const uint32_t INIT_INTERVAL = 100;

static unsigned long lastSensorUpdate = 0;

const uint32_t SENSOR_UPDATE_INTERVAL = 60000;

// ===== INIT =====

void sensors_Init() {

  pH.begin(&Wire);

  Serial.println("pH инициализирован");
  logInfo("pH инициализирован");

  tds.begin(&Wire);

  Serial.println("TDS инициализирован");
  logInfo("TDS инициализирован");

  if (!aht.begin()) {

    Serial.println("Не найден AHT10");
    logError("Не найден AHT10");

  } else {

    Serial.println("AHT10 инициализирован");
    logInfo("AHT10 инициализирован");
  }

  initCounter = 0;

  sensorsReady = false;

  Serial.println("Инициализация датчиков окончена");
  logInfo("Инициализация датчиков окончена");

  Serial.println("Начальное накопление запущено");
  logInfo("Начальное накопление запущено");
}

// ===== SENSOR UPDATE =====

static void updateSensorData() {

  float t = getWaterTemp();

  if (!isnan(t)) {
    Water_temperature = t;
  }

  tds.set_t(Water_temperature);

  PH = medianPH.update(pH.getPH());

  EC = medianEC.update(tds.getEC());

  TDS = medianTDS.update(tds.getTDS());

  sensors_event_t humidity, temp;

  aht.getEvent(&humidity, &temp);

  Air_temperature = temp.temperature;

  Air_humidity = humidity.relative_humidity;
}

// ===== MAIN SENSOR LOOP =====

void sensors_Loop() {

  // ===== INITIAL FILTER FILL =====

  if (!sensorsReady) {

    if (millis() - lastInitRead >= INIT_INTERVAL) {

      lastInitRead = millis();

      updateSensorData();

      initCounter++;

      Serial.printf("Накопление данных: %d/%d\n",
                    initCounter,
                    INIT_READS);

      if (initCounter >= INIT_READS) {

        sensorsReady = true;

        Serial.println("Начальное накопление завершено");
        logInfo("Начальное накопление завершено");
      }
    }

    return;
  }

  // ===== СТАНДАРТНОЕ ОБНОВЛЕНИЕ ДАТЧИКОВ =====

  if (millis() - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL) {

    lastSensorUpdate = millis();

    updateSensorData();
  }
}
