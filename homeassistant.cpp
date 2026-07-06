#include "mqtt.h"
#include "logger.h"
#include "lighting.h"
#include "homeassistant.h"

// ----------------------------

static bool ledState = false;
static bool relayState = false;

// ==========================================================

static void publishState() {
  if (!mqtt_client.connected())
    return;

  mqtt_client.publish(
    "home/aeroponic/switch/led1/state",
    ledState ? "ON" : "OFF",
    true);

  mqtt_client.publish(
    "home/aeroponic/switch/relay1/state",
    relayState ? "ON" : "OFF",
    true);
}

// ==========================================================

static void publishDiscovery() {
  mqtt_client.publish(

    "homeassistant/switch/raintower_led1/config",

    R"rawliteral(
{
"name":"Test LED 1",
"uniq_id":"raintower_led1",

"cmd_t":"home/aeroponic/switch/led1/set",
"stat_t":"home/aeroponic/switch/led1/state",

"pl_on":"ON",
"pl_off":"OFF",

"device":{
"ids":["raintower"],
"name":"RainTower",
"manufacturer":"Alexander",
"model":"ESP32-P4"
}
}
)rawliteral",

    true);

  // -----------------------------------------

  mqtt_client.publish(

    "homeassistant/switch/raintower_relay1/config",

    R"rawliteral(
{
"name":"Test Relay 1",
"uniq_id":"raintower_relay1",

"cmd_t":"home/aeroponic/switch/relay1/set",
"stat_t":"home/aeroponic/switch/relay1/state",

"pl_on":"ON",
"pl_off":"OFF",

"device":{
"ids":["raintower"],
"name":"RainTower",
"manufacturer":"Alexander",
"model":"ESP32-P4"
}
}
)rawliteral",

    true);
}
// ==========================================================

void homeassistant_Init() {
  POWER_KEYS.begin();
  RELAY.begin();
}

// ==========================================================

void homeassistant_OnConnect() {
  mqtt_client.subscribe(
    "home/aeroponic/switch/led1/set");

  mqtt_client.subscribe(
    "home/aeroponic/switch/relay1/set");

  static bool discoverySent = false;

  if (!discoverySent) {
    publishDiscovery();
    discoverySent = true;
  }

  publishState();

  logInfo("Home Assistant подключен");
  Serial.println("Home Assistant подключен");
}

// ==========================================================

void homeassistantCallback(char* topic,
                           uint8_t* payload,
                           unsigned int length) {
  String msg;

  for (unsigned int i = 0; i < length; i++)
    msg += (char)payload[i];

  //----------------------------------------------------

  if (!strcmp(topic,
              "home/aeroponic/switch/led1/set")) {
    ledState = (msg == "ON");

    POWER_KEYS.digitalWrite(1, ledState);

    mqtt_client.publish(
      "home/aeroponic/switch/led1/state",
      ledState ? "ON" : "OFF",
      true);

    return;
  }

  //----------------------------------------------------

  if (!strcmp(topic,
              "home/aeroponic/switch/relay1/set")) {
    relayState = (msg == "ON");

    RELAY.digitalWrite(1, relayState);

    mqtt_client.publish(
      "home/aeroponic/switch/relay1/state",
      relayState ? "ON" : "OFF",
      true);

    return;
  }
}