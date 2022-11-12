#include <Stream.h>
#include "shell.h"

struct ShellRegistryEntry
{
    char *cmd;
    CmdHandler handler;
};

struct ShellBufferEntry
{
    ShellRegistryEntry *cmd;
    char **args;
    int argc;
    Stream *output;
};

const int MAX_BUFFER_COUNT = 20;
const int MAX_CMD_COUNT = 20;
const int MAX_CMD_LENGTH = 1024;

class Shell
{
private:
    ShellRegistryEntry cmdReg[MAX_CMD_COUNT];
    int regCount = 0;

    ShellBufferEntry cmdBuffer[MAX_BUFFER_COUNT];
    int bufferRead = 0;
    int bufferWrite = 0;

    bool parseCmd(char *input, char **out_args, int * out_argc)
    {
        int p = 0;

        // Skip first delimetrs
        while (p < MAX_CMD_LENGTH)
        {
            char c = input[p];
            if (c != ' ') {
                break;
            }
            p++;
        }

        // empty string
        if (p > MAX_CMD_LENGTH) { return false; }

        bool inWord = true;
        *out_argc = 0;
        out_args[*out_argc] = &input[p++];
        
        while (p < MAX_CMD_LENGTH)
        {
            if (input[p] == ' ') {
                input[p] = 0;
                inWord = false;
                p++;
                continue;
            }
            
            if (!inWord) {
                *out_argc = 0;
                out_args[*out_argc] = &input[p++];
                inWord = true;
            }
            p++;
        }
        return true;
    }

public:
    bool registerCmd(char *cmd, CmdHandler handler)
    {
        if (regCount > MAX_CMD_COUNT)
            return false;
        cmdReg[regCount++] = {cmd, handler};
        return true;
    };

    bool enqueueCmd(char *cmd, Stream &out)
    {
        // char * cmd = args[0];
        if (bufferWrite + 1 == bufferRead)
        {
            // buffer full
            // just skip
            return false;
        }
        if (bufferWrite > MAX_BUFFER_COUNT)
        {
            bufferWrite = 0;
        }
        
        char** args;
        int argc;

        parseCmd(cmd, args, &argc);

        for (auto &entry : cmdReg)
        {
            if (strcmp(args[0], entry.cmd))
            {
                cmdBuffer[bufferWrite] = {&entry, args, argc, &out};
                break;
            }
        }
    }

    void begin()
    {
    }

    void loop()
    {
        if (bufferRead != bufferWrite)
        {
            if (bufferRead > MAX_BUFFER_COUNT)
            {
                bufferRead = 0;
            }
            auto cmd = cmdBuffer[bufferRead++];

            if (cmd.cmd)
            {
                cmd.cmd->handler(cmd.args, cmd.argc, *cmd.output);
            }
        }
    }
};
