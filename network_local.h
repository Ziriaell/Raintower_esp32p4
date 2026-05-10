#ifndef NETWORK_H
#define NETWORK_H

#include <ETH.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// extern bool eth_connected;

bool ethernet_Init();
bool check_Wifi();
void networkLoop();

// bool isWifiConnected();
// bool isEthConnected();
// bool isNetworkReady();
// int getActiveInterface();
// bool isNetworkAvailable();
// void onEvent(arduino_event_id_t event);
#endif