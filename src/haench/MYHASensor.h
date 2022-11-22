#pragma once

#include <Arduino.h>
#include <ArduinoHA.h>
#include "MYHAThresholdBase.h"

class MYHASensor : HASensor, MYHAThresholdBase
{
    bool setValueThreshold(const char *value);
};
