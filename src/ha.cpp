#include "ha.h"

HADevice * haDevice;
HAMqtt * mqtt;

HASensor *BoilerStatus;
HASensor *SlaveConfig;
HASensor *SlaveMemeberID;

HASensor *BoilerTemperature;

HASensor *Presure;

HAHVAC *CH;
HAHVAC *DHW;

static HASensor myBoilerStatus("Status");
static HASensor mySlaveConfig("SlaveConfig");
static HASensor mySlaveMemeberID("SlaveMemeberID");

static HASensor myBoilerTemperature("BoilerTemperature");
static HASensor myPresure("CHPresure");

static HAHVAC myCH("CH", HAHVAC::TargetTemperatureFeature | HAHVAC::ModesFeature);
static HAHVAC myDHW("DHW", HAHVAC::TargetTemperatureFeature  | HAHVAC::ModesFeature);

static void onModeCommand(HAHVAC::Mode mode, HAHVAC * sender);

void setupHA(WiFiClient & client) {
    haDevice = new HADevice(UNIQ_ID);
    haDevice->setName("BAXI Slim");
    haDevice->enableSharedAvailability();
    haDevice->enableLastWill();
    
    mqtt = new HAMqtt(client, *haDevice, MAX_DEVICES);
    mqtt->setDiscoveryPrefix(HA_DISCOVERY_PREFIX);
    mqtt->setDataPrefix(HA_DEVICE_PREFIX);
    mqtt->begin(MQTT_BROKER_HOST, MQTT_BROKER_USER, MQTT_BROKER_PASS);

    myBoilerStatus.setName("Status");
    mqtt->addDeviceType(&myBoilerStatus);
    BoilerStatus = & myBoilerStatus;

    mySlaveConfig.setName("Slave config");
    mqtt->addDeviceType(&mySlaveConfig);
    SlaveConfig = &mySlaveConfig;

    mySlaveMemeberID.setName("Slave MemberID");
    mqtt->addDeviceType(&mySlaveMemeberID);
    SlaveConfig = &mySlaveConfig;

    myPresure.setName("CH Presure");
    myPresure.setDeviceClass("pressure");
    myPresure.setUnitOfMeasurement("bar");
    mqtt->addDeviceType(&myPresure);
    Presure = &myPresure;

    myCH.setName("CH");
    myCH.onModeCommand(onModeCommand);
    myCH.setMaxTemp(40);
    myCH.setMinTemp(10);
    myCH.setRetain(true);
    myCH.setTempStep(0.5);
    myCH.setModes(HAHVAC::HeatMode | HAHVAC::OffMode | HAHVAC::ActionFeature);
    mqtt->addDeviceType(&myCH);
    CH = &myCH;

    myDHW.setName("DHW");
    myDHW.onModeCommand(onModeCommand);
    myDHW.setMaxTemp(80);
    myDHW.setMinTemp(10);
    myDHW.setRetain(true);
    myDHW.setTempStep(0.5);
    myDHW.setModes(HAHVAC::HeatMode | HAHVAC::OffMode | HAHVAC::ActionFeature);
    mqtt->addDeviceType(&myDHW);
    DHW = &myDHW;
}

void loopHA() {
    mqtt->loop();
}

static void onModeCommand(HAHVAC::Mode mode, HAHVAC * sender) {
    sender->setMode(mode);
}
