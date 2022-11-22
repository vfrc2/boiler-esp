#include "MYHAThresholdBase.h"

class HAThresholdBase {

private:
    long m_lastUpdate = 0;
    long m_timeThreshold = TIME_THRESHOLD;

    NTPClient *m_ntp;

public:
    bool checkNeedUpdate() {
        if (m_ntp == NULL) return false;
        if ((m_ntp->getEpochTime() - m_lastUpdate) < TIME_THRESHOLD) return false;
        
        return true;
    }

    void setTimeThreshold(long threshold) {
        m_timeThreshold = threshold;
    }

    void setLastUpdate(long time) {
        m_lastUpdate = time;
    }

    void setLastUpdate() {
        m_lastUpdate = m_ntp->getEpochTime();
    }

    void setNTP(NTPClient &ntp) {
        m_ntp = &ntp;
    }
};