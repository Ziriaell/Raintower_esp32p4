#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\config.h"
#ifndef CONFIG_H
#define CONFIG_H

// ==== НАСТРОЙКИ СЕТИ ====
// ==== Ethernet ====
#define ETH_ENABLE true

#define ETH_PHY_ADDR 1
#define ETH_PHY_MDC  31
#define ETH_PHY_MDIO 52
#define ETH_PHY_POWER 51
#define ETH_CLK_MODE EMAC_CLK_EXT_IN

// Название Wi-Fi сети (SSID)
#define WIFI_SSID       "ASUS-WLAN" 
// Пароль для подключения к Wi-Fi
#define WIFI_PASSWORD   "syicm29o"

// ==== SD (SDMMC 4-bit) ====
#define SD_ENABLE true

#define SD_POWER_PIN   45

#define SDMMC_D0   39
#define SDMMC_D1   40
#define SDMMC_D2   41
#define SDMMC_D3   42
#define SDMMC_CMD  44
#define SDMMC_CLK  43

// ==== MQTT настройки ====
// IP-адрес сервера MQTT-брокера
#define MQTT_SERVER     "192.168.1.3"
// Порт, используемый для подключения к MQTT
#define MQTT_PORT       1883
// Идентификатор клиента MQTT
#define MQTT_CLIENT_ID  "Raintower_controller"
// Имя пользователя для авторизации на MQTT-сервере
#define MQTT_USER       "esphome"
// Пароль для подключения к MQTT-серверу
#define MQTT_PASSWORD   "syicm29o"

// ==== OTA настройки ====
// Имя устройства для OTA (обновления по воздуху)
#define OTA_HOSTNAME    "Raintower_controller"
// Пароль для OTA-подключения
#define OTA_PASSWORD    "syicm29o"

// ==== Интервалы ====
// Интервал отправки данных в минутах
#define SENDING_INTERVAL_MINUTES  5
// Интервал проверки включения освещения 
#define LIGHT_CHECK_INTERVAL 20000 //20 секунд
// Требуемое количество минут освещения в день
#define REQUIRED_LIGHT_MINUTES    14 * 60
// Количество измерений для усреднения значения
#define NUM_READ                  30
// Интервал обновления экрана
#define LCD_UPDATE_INTERVAL 1000 // 1 секунда
// Интервал смены страниц на экране
#define PAGE_SWITCH_INTERVAL 5000
// Интервал обновления данных датчиков
#define SENSOR_UPDATE_INTERVAL 10000

// ==== I2C адреса ====
// I2C адрес датчика pH
#define PH_SENSOR_ADDRESS     0x10
// I2C адрес датчика TDS (соли в воде)
#define TDS_SENSOR_ADDRESS    0x11
// I2C адрес экрана
#define LCD_ADDRESS 0x27

// ==== Задействованные пины ====
// Пин ds18b20
#define TEMP_SENSOR_PIN       23
// Пин реле
#define LIGHT_PIN             22

// ==== Геолокация ====
// Географическая широта (для определения времени восхода/заката)
#define LATITUDE 52.58008244648923   // Липецк
// Географическая долгота
#define LONGITUDE 39.50253968208188
// Часовой пояс относительно UTC
#define TIMEZONE 3 


// ==== I2C шина для RTC ====
#define RTC_SDA  17
#define RTC_SCL  16
#endif
