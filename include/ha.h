#pragma once 

#define MAX_DEVICES 40

#include <ArduinoHA.h>
#include "config.h"
#include <ESP8266WiFi.h>

extern HADevice *haDevice;
extern HAMqtt* mqtt;

extern HASensor *BoilerStatus;
extern HASensor *SlaveConfig;
extern HASensor *SlaveMemeberID;
extern HASensor *BoilerTemperature;

extern HASensor *Presure;

extern HAHVAC *CH;
extern HAHVAC *DHW;

void setupHA(WiFiClient & client);
void loopHA();