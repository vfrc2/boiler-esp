#include <ESPAsyncTCP.h>
#include <Arduino.h>
#include "tcp.h"

static std::vector<AsyncClient *> clients; // a list to hold all clients

/* clients events */
static void handleError(void *arg, AsyncClient *client, int8_t error)
{
    Serial.printf("\n connection error %s from client %s \n", client->errorToString(error), client->remoteIP().toString().c_str());
}

static void handleDisconnect(void *arg, AsyncClient *client)
{
    Serial.printf("\n client %s disconnected \n", client->remoteIP().toString().c_str());
}

static void handleTimeOut(void *arg, AsyncClient *client, uint32_t time)
{
    Serial.printf("\n client ACK timeout ip: %s \n", client->remoteIP().toString().c_str());
}

/* server events */
static void handleNewClient(void *arg, AsyncClient *client)
{
    Serial.printf("\n new client has been connected to server, ip: %s", client->remoteIP().toString().c_str());

    // add to list
    clients.push_back(client);

    // register events
    client->onData((AcDataHandler&)arg, NULL);
    client->onError(&handleError, NULL);
    client->onDisconnect(&handleDisconnect, NULL);
    client->onTimeout(&handleTimeOut, NULL);
}

void setupTCP(void * dataHandler)
{
    AsyncServer *server = new AsyncServer(23);
    server->onClient(&handleNewClient, ((void *)dataHandler));
    server->begin();
}
