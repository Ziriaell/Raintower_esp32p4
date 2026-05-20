#ifndef SENSORS_H
#define SENSORS_H

extern float PH, EC, TDS, Water_temperature, Air_temperature, Air_humidity;

void sensors_Init();
void sensors_Loop();

#endif