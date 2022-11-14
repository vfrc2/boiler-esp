#pragma once 

#include <ArduinoHA.h>

void initConfig(HADevice & device, HAMqtt & mqtt);

void readLoop();