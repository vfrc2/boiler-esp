#pragma once

#include <NTPClient.h>

#define TIME_THRESHOLD (60 * 1000)

class MYHAThresholdBase {
    public: 
        void setNTP(NTPClient &ntp);
        bool checkNeedUpdate();
        void setLastUpdate();
        void setLastUpdate(long time);
        void setTimeThreshold(long threshold);

};