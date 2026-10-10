#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\logger.h"
#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>


bool logger_Init();
void logger_Close();

void loggerSetRTCReady(bool ready);

void logInfo(const String& msg);
void logError(const String& msg);
void logDebug(const String& msg);

#endif