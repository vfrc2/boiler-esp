#include <Arduino.h>
#include <ArduinoHA.h>
#include "MYHAThresholdBase.h"


class MYHASensor : HASensor, MYHAThresholdBase
{
    char *m_lastValue = NULL;

    bool setValueThreshold(const char *value)
    {
        bool needUpdate = checkNeedUpdate();

        if (strncmp(m_lastValue, value, 2000) == 0)
        {
           needUpdate = false;
        }

        if (needUpdate) {
            setLastUpdate();
            return setValue(value);
        }

        return false;
    }
};
