#pragma once

#include "Arduino.h"

void cmdSetup();

int pushCmd(char * data, int len, Print& output);

int pushCmd(char* cmd, char** args, int argc, Print& output);

void cmdLoop();