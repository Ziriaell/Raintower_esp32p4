#include <ArduinoJson.h>

#include "homeassistant.h"
#include "mqtt.h"
#include "actuators.h"
#include "logger.h"

// ---------- описание MQTT switch ----------

struct HASwitch {
  const char* name;
  const char* uniqueId;

  const char* commandTopic;
  const char* stateTopic;

  uint8_t channel;
  bool relay;
};

// ---------- список устройств ----------

static const HASwitch switches[] = {

  { "PowerKey Channel 1",
    "raintower_led1",
    "home/aeroponic/switch/led1/set",
    "home/aeroponic/switch/led1/state",
    1, false },

  { "PowerKey Channel 2",
    "raintower_led2",
    "home/aeroponic/switch/led2/set",
    "home/aeroponic/switch/led2/state",
    2, false },

  { "PowerKey Channel 3",
    "raintower_led3",
    "home/aeroponic/switch/led3/set",
    "home/aeroponic/switch/led3/state",
    3, false },

  { "PowerKey Channel 4",
    "raintower_led4",
    "home/aeroponic/switch/led4/set",
    "home/aeroponic/switch/led4/state",
    4, false },

  { "Relay Channel 1",
    "raintower_relay1",
    "home/aeroponic/switch/relay1/set",
    "home/aeroponic/switch/relay1/state",
    1, true },

  { "Relay Channel 2",
    "raintower_relay2",
    "home/aeroponic/switch/relay2/set",
    "home/aeroponic/switch/relay2/state",
    2, true }
};

constexpr uint8_t SWITCH_COUNT =
  sizeof(switches) / sizeof(switches[0]);

// ---------------------------------------------------

static void publishState(const HASwitch& sw, bool state) {
  mqtt_client.publish(
    sw.stateTopic,
    state ? "ON" : "OFF",
    true);
}

// ---------------------------------------------------

static void publishAllStates() {
  for (uint8_t i = 0; i < SWITCH_COUNT; i++) {
    bool state;

    if (switches[i].relay)
      state = GetRelay(
        switches[i].channel);
    else
      state = GetPowerKey(
        switches[i].channel);

    publishState(
      switches[i],
      state);
  }
}

static void publishDiscovery(const HASwitch& sw) {
  StaticJsonDocument<512> doc;

  doc["name"] = sw.name;
  doc["uniq_id"] = sw.uniqueId;

  doc["cmd_t"] = sw.commandTopic;
  doc["stat_t"] = sw.stateTopic;

  doc["pl_on"] = "ON";
  doc["pl_off"] = "OFF";

  JsonObject device =
    doc.createNestedObject("device");

  device["ids"] = "raintower_actuators";
  device["name"] = "RainTower Actuators";
  device["manufacturer"] = "Alexander";
  device["model"] = "ESP32-P4";

  char topic[80];

  sprintf(topic,
          "homeassistant/switch/%s/config",
          sw.uniqueId);

  char payload[512];

  serializeJson(doc, payload);

  mqtt_client.publish(
    topic,
    payload,
    true);
}

// ---------------------------------------------------

static void publishAllDiscovery() {
  for (uint8_t i = 0; i < SWITCH_COUNT; i++)
    publishDiscovery(switches[i]);
}

// ---------------------------------------------------


void homeassistant_OnConnect() {
  for (uint8_t i = 0; i < SWITCH_COUNT; i++)
    mqtt_client.subscribe(switches[i].commandTopic);

  publishAllDiscovery();

  logInfo("Home Assistant connected");
}

// ---------------------------------------------------

void homeassistantCallback(
  char* topic,
  uint8_t* payload,
  unsigned int length) {
  String command;

  for (unsigned int i = 0; i < length; i++)
    command += (char)payload[i];

  bool on = (command == "ON");

  // Ищем описание переключателя
  const HASwitch* sw = nullptr;

  for (uint8_t i = 0; i < SWITCH_COUNT; i++) {

    if (strcmp(topic, switches[i].commandTopic) == 0) {

      sw = &switches[i];
      break;
    }
  }

  // Такой топик нас не интересует
  if (sw == nullptr) {
    return;
  }

  // Выполняем команду
  if (sw->relay) {

    SetRelay(
      sw->channel,
      on);

  } else {

    SetPowerKey(
      sw->channel,
      on);
  }

  // Отправляем подтверждение состояния
  publishState(*sw, on);
}