#include <ESP8266WebServer.h>

#include "telnet.h"

static WiFiServer shellServer(23);
static WiFiClient shellClient;

static Shell* shell;
static bool haveClient;

void setupTelnet(Shell& ashell) {
    shellServer.begin();
    haveClient = false;
    shell = &ashell;
}

void loopTelnet() {
     if (!haveClient)
    {
        // Check for new client connections.
        shellClient = shellServer.available();
        if (shellClient)
        {
            haveClient = true;
            shell->begin(shellClient, 10, Terminal::Mode::Telnet);
        }
    }
    else if (!shellClient.connected())
    {
        // The current client has been disconnected.  Shut down the shell.
        shell->end();
        shellClient.stop();
        shellClient = WiFiClient();
        haveClient = false;
    }

    // Perform periodic shell processing on the active client.
    shell->loop();
}