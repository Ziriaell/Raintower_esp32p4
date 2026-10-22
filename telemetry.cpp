
#include "telemetry.h"

#include <Arduino.h>
#include <math.h>
#include <stdio.h>

#include "mqtt.h"
#include "sensors.h"
#include "logger.h"
#include "config.h"
#include "rtc.h"
#include "network_local.h"

static unsigned long lastSend = 0;

// ===== ПУБЛИКАЦИЯ ЧИСЛОВЫХ ЗНАЧЕНИЙ =====

static void publishFloat(
    const char* topic,
    float value,
    uint8_t precision = 2)
{
    if (!mqtt_client.connected() || isnan(value)) {
        return;
    }

    char payload[24];

    dtostrf(value, 1, precision, payload);

    mqtt_client.publish(topic, payload, true);
}

static void publishUnsigned(
    const char* topic,
    unsigned long value)
{
    if (!mqtt_client.connected()) {
        return;
    }

    char payload[24];

    snprintf(payload, sizeof(payload), "%lu", value);

    mqtt_client.publish(topic, payload, true);
}

// ===== ПУБЛИКАЦИЯ ТЕКСТОВЫХ ЗНАЧЕНИЙ =====

static void publishText(
    const char* topic,
    const char* value)
{
    if (!mqtt_client.connected()) {
        return;
    }

    mqtt_client.publish(topic, value, true);
}

// ===== СИСТЕМНОЕ ВРЕМЯ =====

static void publishSystemTime()
{
    if (!isTimeValid()) {
        publishText(
            "home/raintower/system/time",
            "unavailable");
        return;
    }

    DateTime now = getNow();

    char payload[24];

    snprintf(
        payload,
        sizeof(payload),
        "%04d-%02d-%02d %02d:%02d:%02d",
        now.year(),
        now.month(),
        now.day(),
        now.hour(),
        now.minute(),
        now.second());

    publishText(
        "home/raintower/system/time",
        payload);
}

// ===== СЕТЕВАЯ ТЕЛЕМЕТРИЯ =====

static void publishNetworkTelemetry()
{
    const bool eth = isEthConnected();
    const bool wifi = isWifiConnected();

    publishText(
        "home/raintower/system/ethernet_connected",
        eth ? "ON" : "OFF");

    publishText(
        "home/raintower/system/wifi_connected",
        wifi ? "ON" : "OFF");

    publishText(
        "home/raintower/system/ethernet_ip",
        eth ? ETH.localIP().toString().c_str() : "OFF");

    publishText(
        "home/raintower/system/wifi_ip",
        wifi ? WiFi.localIP().toString().c_str() : "OFF");

    // Это предпочтительный интерфейс по текущей логике сети,
    // а не гарантированное определение маршрута MQTT-соединения.
    const char* preferredInterface =
        eth ? "ETH" :
        wifi ? "WIFI" :
        "OFF";

    publishText(
        "home/raintower/system/preferred_interface",
        preferredInterface);

    if (wifi) {
        publishFloat(
            "home/raintower/system/wifi_rssi",
            (float)WiFi.RSSI(),
            0);
    }
}

// ===== СИСТЕМНАЯ ТЕЛЕМЕТРИЯ =====

static void publishSystemTelemetry()
{
    // Время работы в секундах.
    // Используем тот же источник времени работы, что и LCD.
    publishUnsigned(
        "home/raintower/system/uptime",
        millis() / 1000UL);

    // Свободная память в байтах.
    publishUnsigned(
        "home/raintower/system/free_heap",
        ESP.getFreeHeap());

    // Свободная PSRAM в байтах.
    publishUnsigned(
        "home/raintower/system/free_psram",
        ESP.getFreePsram());

    publishSystemTime();
    publishNetworkTelemetry();
}

// ===== ОСНОВНОЙ ЦИКЛ ТЕЛЕМЕТРИИ =====

void telemetry_Loop()
{
    if (millis() - lastSend <
        SENDING_INTERVAL_MINUTES * 60000UL) {
        return;
    }

    lastSend = millis();

    if (!mqtt_client.connected()) {
        return;
    }

    // ===== ВОДА =====

    publishFloat(
        "home/raintower/sensors/water_temperature",
        Water_temperature);

    publishFloat(
        "home/raintower/sensors/ph",
        PH);

    publishFloat(
        "home/raintower/sensors/ec",
        EC);

    publishFloat(
        "home/raintower/sensors/tds",
        TDS);

    // ===== ВОЗДУХ =====

    publishFloat(
        "home/balcony/sensors/air_temperature",
        Air_temperature);

    publishFloat(
        "home/balcony/sensors/air_humidity",
        Air_humidity);

    // ===== СИСТЕМА =====

    publishSystemTelemetry();

    logInfo("Телеметрия отправлена");
}