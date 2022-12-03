#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <GyverOS.h>
#include <Shell.h>
#include "config.h"

#include "ha.h"
#include "ot.h"
#include "sensors.h"
#include "telnet.h"

GyverOS<5> os;
WiFiClient client;

Shell serialShell;
Shell telnetShell;

void setup()
{
    Serial.begin(9600);
    serialShell.begin(Serial, 5, Terminal::Serial);

    delay(1000);
    Serial.println("\nStart\n");
    Serial.printf("Connecting to %s", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500); // waiting for the connection
    }
    Serial.print("\n");
    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());

    // sensor ot
    os.attach(
        0, []()
        { readOTLoop(); },
        5 * 1000);

    // sensor analog
    os.attach(
        1, []()
        { readAnalogSensors(); },
        5 * 60 * 1000);

    setupHA(client);
    setupOT();
    setupTelnet(telnetShell);

    CH->onTargetTemperatureCommand(onTargetTemperatureCommand);
    DHW->onTargetTemperatureCommand(onTargetTemperatureCommand);

    os.exec(1);
}

void loop()
{
    os.tick();

    serialShell.loop();
    telnetShell.loop();
    loopHA();
}
