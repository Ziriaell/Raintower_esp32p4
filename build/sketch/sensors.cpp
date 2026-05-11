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

float PH, EC, TDS, Water_temperature, Air_temperature, Air_humidity;

Adafruit_AHT10 aht;
iarduino_I2C_pH pH(PH_SENSOR_ADDRESS);
iarduino_I2C_TDS tds(TDS_SENSOR_ADDRESS);

class MedianFilter {
private:
  float buffer[NUM_READ];
  int count;

public:
  MedianFilter() {
    for (int i = 0; i < NUM_READ; i++) buffer[i] = 0.0;
    count = 0;
  }

  float update(float newVal) {
    buffer[count] = newVal;
    count = (count + 1) % NUM_READ;

    float sorted[NUM_READ];
    memcpy(sorted, buffer, sizeof(buffer));

    std::sort(sorted, sorted + NUM_READ);

    if (NUM_READ % 2 == 0)
      return (sorted[NUM_READ / 2 - 1] + sorted[NUM_READ / 2]) / 2.0;
    else
      return sorted[NUM_READ / 2];
  }
};

MedianFilter medianPH, medianEC, medianTDS;

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
    while (1) delay(10);
  }
  Serial.println("AHT10 инициализирован");
  logInfo("AHT10 инициализирован");
  // Optional: force resync even if power not lost
  // syncRTCwithNTP();
  Serial.println("Инициализация датчиков окончена");
  logInfo("Инициализация датчиков окончена");
  for (int i = 0; i < NUM_READ + NUM_READ; i++) {
    // medianWT.update(getWaterTemp());
    medianPH.update(pH.getPH());
    medianEC.update(tds.getEC());
    medianTDS.update(tds.getTDS());
    delay(100);
  }
  Serial.println("Начальное накопление завершено");
  logInfo("Начальное накопление завершено");
}

void updateSensorData() {
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
