#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\mqtt.h"
#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
#include "network_local.h"
#include "config.h"

extern NetworkClient network_Client;
extern PubSubClient mqtt_client;

void mqtt_Init();
void mqtt_Loop();
void mqtt_Reconnect();
void publishFloat();

#endif
