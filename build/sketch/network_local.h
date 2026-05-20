#line 1 "C:\\Users\\User\\Documents\\Repo\\Raintower_esp32p4\\network_local.h"
#ifndef NETWORK_H
#define NETWORK_H

#include <ETH.h>
#include <WiFi.h>
#include <WiFiUdp.h>

bool ethernet_Init();
bool check_Wifi();
void network_Loop();
bool waitForWifi();

bool isWifiConnected();
bool isEthConnected();

#endif