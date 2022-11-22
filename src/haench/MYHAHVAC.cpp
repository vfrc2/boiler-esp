#pragma once

#include <Arduino.h>
#include <ArduinoHA.h>
#include "MYHAThresholdBase.h"


class MYHASensor : HAHVAC, MYHAThresholdBase
{
    const HANumeric *m_lastTemperatureValue = NULL;

    bool setCurrentTemperature(const HANumeric &temperature, const bool force = false)
    {
        bool needUpdate = checkNeedUpdate();

        if (needUpdate)
        {
            setLastUpdate();
            m_lastTemperatureValue = &temperature;
            return setCurrentTemperature(temperature, force);
        }

        return false;
    }
};
