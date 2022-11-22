#pragma once

#include <Arduino.h>
#include <ArduinoHA.h>
#include "MYHAThresholdBase.h"

#define _SET_CURRENT_TEMPERATURE_THRESHOLD_OVERLOAD(type)                                                  \
    /** @overload */                                                                             \
    inline bool setCurrentTemperatureThreshold(const type temperature, const bool force = false) \
    {                                                                                            \
        return setCurrentTemperatureThreshold(HANumeric(temperature, NumberPresi), force);        \
    }


class MYHASensor : HAHVAC, MYHAThresholdBase
{
    char *m_lastValue = NULL;

    bool setCurrentTemperatureThreshold(const HANumeric& temperature, const bool force = false);

    _SET_CURRENT_TEMPERATURE_THRESHOLD_OVERLOAD(int8_t)
    _SET_CURRENT_TEMPERATURE_THRESHOLD_OVERLOAD(int16_t)
    _SET_CURRENT_TEMPERATURE_THRESHOLD_OVERLOAD(int32_t)
    _SET_CURRENT_TEMPERATURE_THRESHOLD_OVERLOAD(uint8_t)
    _SET_CURRENT_TEMPERATURE_THRESHOLD_OVERLOAD(uint16_t)
    _SET_CURRENT_TEMPERATURE_THRESHOLD_OVERLOAD(uint32_t)
    _SET_CURRENT_TEMPERATURE_THRESHOLD_OVERLOAD(float)
};
