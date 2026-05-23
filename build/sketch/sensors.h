#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\sensors.h"
#ifndef SENSORS_H
#define SENSORS_H

extern float PH, EC, TDS, Water_temperature, Air_temperature, Air_humidity;

void sensors_Init();
void sensors_Loop();

#endif