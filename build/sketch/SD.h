#line 1 "C:\\Users\\User\\Documents\\Repo\\Raintower_esp32p4\\SD.h"
#ifndef SD_H
#define SD_H

bool SD_Init();
void logToSD(const char* tag, String message);

#endif