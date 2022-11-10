#include "cmd.h"
#include "Arduino.h"

const int MAX_ARGC = 25;

struct Command
{
    char **m_args;
    int m_argc;
    Print &m_output;

    Command(char **args, int argc, Print &output) : m_args(args),
                                                    m_argc(argc),
                                                    m_output(output) {}
};

const int maxCommands = 10;
int readCommand = 0;
int writeCommand = 0;

Command *cmdBuffer[maxCommands];

void cmdSetup()
{
    readCommand = 0;
    writeCommand = 0;
};


int pushCmd(char **args, int argc, Print &output)
{
    if (writeCommand + 1 == readCommand)
    {
        // full
        return 1;
    }

    if (writeCommand > maxCommands)
    {
        writeCommand = 0;
    }

    cmdBuffer[writeCommand++] = new Command(args, argc, output);

    return 0;
};

int pushCmd(char *buffer, int len, Print &output)
{
    char *args[MAX_ARGC];
    int argc = 0;

    bool inDelimetr = false;
    bool inWord = false;

    for (int i = 0; i < len; i++)
    {
        char c = buffer[i];
        if (inWord && c == ' ')
        {
            buffer[i] = 0;
        }
        if (inDelimetr && c == ' ')
        {
            continue;
        }
        if (c == ' ')
        {
            inWord = false;
            inDelimetr = true;
            continue;
            ;
        }
        if (!inWord)
        {
            inWord = true;
            inDelimetr = false;
            args[argc++] = &buffer[i];
            if (argc > MAX_ARGC)
            {
                // just skip rest args
                break;
            }
            continue;
        }
        // inWord but not delimetr just continue
    }

    return pushCmd(args, argc, output);
}

void cmdLoop()
{
    if (readCommand != writeCommand)
    {
        if (readCommand > maxCommands)
        {
            readCommand = 0;
        }
        Command *cmd = cmdBuffer[readCommand++];

        Serial.print(cmd->m_args[0]);

        delete cmd;
    }
};