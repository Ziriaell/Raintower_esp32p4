#include "network_local.h"
#include "mqtt.h"
#include "logger.h"
#include "config.h"

// bool eth_connected = false;

static bool eth_connected = false;
static int lastIf = 0;
static unsigned long lastReconnect = 0;

extern void mqtt_Reconnect();

bool isWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool isEthConnected() {
  return eth_connected;
}

static bool isNetworkReady() {
  return isWifiConnected() || isEthConnected();
}

static int getActiveInterface() {
  if (isEthConnected()) return 2;
  if (isWifiConnected()) return 1;
  return 0;
}

static void onEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH запущена");
      logInfo("ETH запущена");
      ETH.setHostname("raintower");
      break;

    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH подключен");
      logInfo("ETH подключен");
      break;

    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("ETH IP: ");
      Serial.println(ETH.localIP());
      logInfo("ETH IP: " + ETH.localIP().toString());
      eth_connected = true;
      break;

    case ARDUINO_EVENT_ETH_LOST_IP:
      Serial.println("ETH потеря IP");
      logError("ETH потеря IP");
      eth_connected = false;
      break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH соединение разорвано");
      logError("ETH соединение разорвано");
      eth_connected = false;
      break;

    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      // logError("ETH остановлен");
      eth_connected = false;
      break;

    default:
      break;
  }
}

bool ethernet_Init() {
  Network.onEvent(onEvent);
  pinMode(ETH_PHY_POWER, OUTPUT);
  digitalWrite(ETH_PHY_POWER, HIGH);
  delay(100);

  ETH.begin();

  return true;
}

bool check_Wifi() {
  return WiFi.status() == WL_CONNECTED;
}

// bool check_Wifi() {
//   Serial.print("Проверка подключения к Wi-Fi ");
//     logInfo("Проверка подключения к Wi-Fi ");
//   int attempts = 0;
//   while (WiFi.status() != WL_CONNECTED && attempts < 40) {
//   // while (WiFi.status() != WL_CONNECTED) {
//     delay(250);
//     Serial.println(".");
//     attempts++;
//   }
//   if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("Подключено!");
//     logInfo("Подключено!");
//     return true;
//   } else {
//     Serial.println(" Не удалось подключиться.");
//     logError(" Не удалось подключиться.");
//     return false;
//   }
// }

bool waitForWifi() {

  Serial.print("Проверка подключения к Wi-Fi ");
  logInfo("Проверка подключения к Wi-Fi ");
  uint8_t attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 50) {

    delay(250);

    Serial.print(".");

    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("Подключено!");
    logInfo("Подключено!");

    return true;
  } else {
    Serial.println(" Не удалось подключиться.");
    logError(" Не удалось подключиться.");
    return false;
  }
}
// bool waitForWifi() {

//   static bool started = false;
//   static unsigned long lastPrint = 0;
//   static uint8_t attempts = 0;

//   if (WiFi.status() == WL_CONNECTED) {

//     if (started) {

//       Serial.println("Подключено!");
//       logInfo("Подключено!");


//       started = false;
//     }

//     return true;
//   }

//   if (!started) {

//     Serial.print("Проверка подключения к Wi-Fi ");
//     logInfo("Проверка подключения к Wi-Fi ");

//     started = true;
//     attempts = 0;
//     lastPrint = millis();
//   }

//   if (millis() - lastPrint >= 250) {

//     Serial.print(".");

//     lastPrint = millis();

//     attempts++;
//   }

//   if (attempts >= 50) {

//     Serial.println(" Не удалось подключиться.");
//     logError(" Не удалось подключиться.");

//     started = false;

//     return false;
//   }

//   return false;
// }
// ===== МОНИТОРИНГ =====

extern void mqtt_Reconnect();

void network_Loop() {

  int nowIf = getActiveInterface();

  if (nowIf != lastIf && lastIf != 0) {

    Serial.println("Сетевой интерфейс изменен");
    logDebug("Сетевой интерфейс изменен");

    // reconnect только если сеть есть
    if (nowIf != 0) {

      if (millis() - lastReconnect > 5000) {
        mqtt_Reconnect();
        lastReconnect = millis();
      }
    }
  }

  lastIf = nowIf;
}