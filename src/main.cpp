#include <Arduino.h>

#define ARDUINOHA_DEBUG
#define DEBUG_ENABLED

#include "debug.h"
#include <ESP8266WiFi.h>
#include <ArduinoHA.h>

#define HA_DISCOVERY_PREFIX "homeassistant"
#define HA_DEVICE_PREFIX "esphome"

#include "config.h"

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

void setup()
{
    Serial.begin(9600);
    delay(1000);
    Serial.println("\nStart\n");
    Serial.printf("Connecting to %s", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500); // waiting for the connection
    }
    Serial.print("\n");

    Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());

    // device.enableSharedAvailability();
    // device.enableLastWill();

    // mqtt.setDiscoveryPrefix(HA_DISCOVERY_PREFIX);
    // mqtt.setDataPrefix(HA_DEVICE_PREFIX);
    mqtt.begin(MQTT_BROKER_HOST, MQTT_BROKER_USER, MQTT_BROKER_PASS);
}

void loop()
{
    // system tasks

    // check wifi
    if (WiFi.status() != WL_CONNECTED) {
        DEBUG("Connection lost, try to reconnect...\n");
        return;
    }

    // network stuff
    mqtt.loop();
}
