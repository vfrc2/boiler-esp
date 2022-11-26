#include "ot.h"

OpenTherm * ot;

static void IRAM_ATTR handleInterrupt()
{
    ot->handleInterrupt();
}

void setupOT() {
    ot = new OpenTherm(INPIN, OUTPIN);
    ot->begin(handleInterrupt);
}

unsigned long otBuildRequest(OpenThermMessageType type, uint8_t id, unsigned int data)
{
    unsigned long request = data;
    if (type == OpenThermMessageType::WRITE_DATA)
    {
        request |= 1ul << 28;
    }
    request |= ((unsigned long)id) << 16;
    if (ot->parity(request))
        request |= (1ul << 31);
    return request;
};