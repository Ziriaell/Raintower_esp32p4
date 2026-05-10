#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
#include "network_local.h"
#include "config.h"

extern WiFiClient network_Client;
extern PubSubClient mqtt_client;

void mqtt_Init();
void mqtt_Reconnect();
void mqtt_Loop();

#endif
