#line 1 "C:\\Users\\Горяйнов Александр\\Documents\\Git\\Raintower_esp32p4\\network_local.h"
#ifndef NETWORK_H
#define NETWORK_H

#include <ETH.h>
#include <WiFi.h>
#include <WiFiUdp.h>

bool ethernet_Init();
void network_Loop();
bool waitForWifi();

bool isWifiConnected();
bool isEthConnected();
bool isNetworkReady();

#endif