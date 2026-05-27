#include "telemetry.h"

#include "mqtt.h"
#include "sensors.h"
#include "logger.h"
#include "config.h"

static unsigned long lastSend = 0;

static void publishFloat(const char* topic,
                         float value,
                         uint8_t precision = 2)
{
    if (!mqtt_client.connected()) {
        return;
    }

    if (isnan(value)) {
        return;
    }

    char payload[16];

    dtostrf(value, 1, precision, payload);

    mqtt_client.publish(topic, payload, true);
}

void telemetry_Loop()
{
    if (millis() - lastSend <
        SENDING_INTERVAL_MINUTES * 60000UL) {
        return;
    }

    lastSend = millis();

    // ===== вода =====

    publishFloat(
        "home/aeroponic/sensors/water_temperature",
        Water_temperature);

    publishFloat(
        "home/aeroponic/sensors/ph",
        PH);

    publishFloat(
        "home/aeroponic/sensors/ec",
        EC);

    publishFloat(
        "home/aeroponic/sensors/tds",
        TDS);

    // ===== воздух =====

    publishFloat(
        "home/balcony/sensors/air_temperature",
        Air_temperature);

    publishFloat(
        "home/balcony/sensors/air_humidity",
        Air_humidity);
    // ===== всякое =====

    
    logInfo("Телеметрия отправлена");
}