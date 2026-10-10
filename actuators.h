#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <sunset.h>
#include "rtc.h"

extern int lastSyncedDay;
extern SunSet sun;

// extern iarduino_I2C_Relay POWER_KEYS;
// extern iarduino_I2C_Relay RELAY;

void actuators_Init();
void calculateLightingNeeds(DateTime now);
void controlLighting(DateTime now);

void SetPowerKey(uint8_t channel, bool state);
void SetRelay(uint8_t channel, bool state);

bool GetPowerKey(uint8_t channel);
bool GetRelay(uint8_t channel);

#endif