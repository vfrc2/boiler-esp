#include "ot.h"

OpenTherm * ot;

void IRAM_ATTR handleInterrupt()
{
    ot->handleInterrupt();
}

void setupOT() {
    ot = new OpenTherm(INPIN, OUTPIN);
    ot->begin(handleInterrupt);
}
