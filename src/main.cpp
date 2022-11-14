#include <Arduino.h>

#define ARDUINOHA_DEBUG
#define DEBUG_ENABLED

#include "debug.h"
#include <ESP8266WiFi.h>
#include <ArduinoHA.h>
#include <OpenTherm.h>

#define HA_DISCOVERY_PREFIX "homeassistant"
#define HA_DEVICE_PREFIX "esphome"
#define UNIQ_ID "boiler"

#include "config.h"
#include "sensors.h"

const int inPin = 4;  // for Arduino, 4 for ESP8266 (D2), 21 for ESP32
const int outPin = 5; // for Arduino, 5 for ESP8266 (D1), 22 for ESP32

OpenTherm ot(inPin, outPin);
WiFiClient client;
HADevice device(UNIQ_ID);
HAMqtt mqtt(client, device, 30);

void ICACHE_RAM_ATTR handleInterrupt()
{
    ot.handleInterrupt();
}

void setup()
{
    Serial.begin(9600);

    ot.begin(handleInterrupt);

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

    // device.setName("BAXI Slim");

    mqtt.setDiscoveryPrefix(HA_DISCOVERY_PREFIX);
    mqtt.setDataPrefix(HA_DEVICE_PREFIX);
    mqtt.begin(MQTT_BROKER_HOST, MQTT_BROKER_USER, MQTT_BROKER_PASS);
    
    device.setName("BAXI Slim");
    device.enableSharedAvailability();
    device.enableLastWill();
    initConfig(device, mqtt);
}

void loop()
{
    // system tasks
    readLoop();
    // check wifi
    // network stuff
    mqtt.loop();
}
