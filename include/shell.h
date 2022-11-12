#pragma once

#include <Stream.h>

typedef void (*CmdHandler)(char *, char, Stream &);

const int MAX_BUFFER_COUNT = 20;
const int MAX_CMD_COUNT = 20;
const int MAX_CMD_LENGTH = 1024;

class Shell
{
public:
    bool registerCmd(char *cmd, CmdHandler handler);
    bool enqueueCmd(char *cmd, Stream &out);
    void begin();
    void loop();
};
