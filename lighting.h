#ifndef LIGHTING_H
#define LIGHTING_H

#include <iarduino_I2C_Relay.h>
#include <sunset.h>
#include "rtc.h"

extern int lastSyncedDay;
extern SunSet sun;

extern iarduino_I2C_Relay POWER_KEYS;
extern iarduino_I2C_Relay RELAY;

void lighting_Init();
void calculateLightingNeeds(DateTime now);
void controlLighting(DateTime now);

#endif