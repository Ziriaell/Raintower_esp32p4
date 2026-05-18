#line 1 "C:\\Users\\User\\Documents\\Repo\\Raintower_esp32p4\\mqtt.cpp"
#include "mqtt.h"
#include "logger.h"

NetworkClient network_Client;
PubSubClient mqtt_client(network_Client);

static unsigned long lastReconnectAttempt = 0;

void mqtt_Init(){
  mqtt_client.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt_client.connected();
}

void mqtt_Reconnect() {

    // если уже подключены — ничего не делаем
    if (mqtt_client.connected()) {
        return;
    }

    // защита от частых попыток
    if (millis() - lastReconnectAttempt < 5000) {
        return;
    }

    lastReconnectAttempt = millis();

    check_Wifi();

    Serial.print("Подключение к MQTT серверу...");
    logInfo("Подключение к MQTT серверу...");

    if (mqtt_client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {

        Serial.println("Подключено");
        logInfo("MQTT подключен");

        // подписки
        // mqtt_client.subscribe(...);

    } else {

        Serial.print("Ошибка MQTT, rc=");
        Serial.println(mqtt_client.state());

        char buf[50];
        snprintf(buf, sizeof(buf),
                 "MQTT connect failed rc=%d",
                 mqtt_client.state());

        logError(buf);
    }
}

void mqtt_Loop() {

    if (!mqtt_client.connected()) {
        mqtt_Reconnect();
    }

    mqtt_client.loop();
}