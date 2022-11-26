#pragma once 

#include <OpenTherm.h>

#define INPIN 4  // for Arduino, 4 for ESP8266 (D2), 21 for ESP32
#define OUTPIN 5 // for Arduino, 5 for ESP8266 (D1), 22 for ESP32

extern OpenTherm * ot;

void setupOT();
