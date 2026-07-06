#ifndef HOMEASSISTANT_H
#define HOMEASSISTANT_H

#include <Arduino.h>

void homeassistant_Init();
void homeassistant_OnConnect();

void homeassistantCallback(char* topic,
                           uint8_t* payload,
                           unsigned int length);

#endif