#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\lighting.h"
#ifndef LIGHTING_H
#define LIGHTING_H

#include <sunset.h>
#include "rtc.h"

extern int lastSyncedDay;

extern SunSet sun;

void lighting_Init();
void calculateLightingNeeds(DateTime now);
void controlLighting(DateTime now);

#endif