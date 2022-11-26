#include "ha.h"

HADevice * haDevice;
HAMqtt * mqtt;

HASensor *FaultIndication;
HASensor *CHMode;
HASensor *DHWMode;
HASensor *FlameStatus;
HASensor *CoolingStatus;
HASensor *CH2Mode;
HASensor *DiagnosticIndication;

HASensor *SlaveConfig;
HASensor *SlaveMemeberID;

HASensor *BoilerTemperature;

HASensor *Presure;

HAHVAC *CH;
HAHVAC *DHW;

static HASensor myFaultIndication("FaultIndication");
static HASensor myCHMode("CHMode");
static HASensor myDHWMode("DHWMode");
static HASensor myFlameStatus("FlameStatus");
static HASensor myCoolingStatus("CoolingStatus");
static HASensor myCH2Mode("CH2Mode");
static HASensor myDiagnosticIndication("DiagnosticIndication");

static HASensor mySlaveConfig("SlaveConfig");
static HASensor mySlaveMemeberID("SlaveMemeberID");

static HASensor myBoilerTemperature("BoilerTemperature");

static HASensor myPresure("CHPresure");

static HAHVAC myCH("CH", HAHVAC::TargetTemperatureFeature | HAHVAC::PowerFeature | HAHVAC::ModesFeature);
static HAHVAC myDHW("DHW", HAHVAC::TargetTemperatureFeature | HAHVAC::PowerFeature | HAHVAC::ModesFeature);

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

     // LB Slave status
    myFaultIndication.setName("Fault Indication");
    mqtt->addDeviceType(&myFaultIndication);
    FaultIndication = & myFaultIndication;

    myCHMode.setName("CH Mode");
    mqtt->addDeviceType(&myCHMode);
    CHMode = & myCHMode;

    myDHWMode.setName("DHW Mode");
    mqtt->addDeviceType(&myDHWMode);
    DHWMode = &myDHWMode;

    myFlameStatus.setName("Flame status");
    mqtt->addDeviceType(&myFlameStatus);
    FlameStatus = &myFlameStatus;

    myCoolingStatus.setName("Cooling status");
    mqtt->addDeviceType(&myCoolingStatus);
    CoolingStatus = &myCoolingStatus;

    myCH2Mode.setName("CH2 mode");
    mqtt->addDeviceType(&myCH2Mode);
    CH2Mode = &myCH2Mode;

    myDiagnosticIndication.setName("Diagnostic indication");
    mqtt->addDeviceType(&myDiagnosticIndication);
    DiagnosticIndication = &myDiagnosticIndication;

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
    myCH.setTargetTemperature(21);
    myCH.setMaxTemp(80);
    myCH.setMinTemp(10);
    myCH.setRetain(true);
    myCH.setTempStep(0.5);
    myCH.setModes(HAHVAC::HeatMode | HAHVAC::OffMode);
    mqtt->addDeviceType(&myCH);
    CH = &myCH;

    myDHW.setName("DHW");
    myDHW.onModeCommand(onModeCommand);
    myDHW.setTargetTemperature(21);
    myDHW.setMaxTemp(80);
    myDHW.setMinTemp(10);
    myDHW.setRetain(true);
    myDHW.setTempStep(0.5);
    myDHW.setModes(HAHVAC::HeatMode | HAHVAC::OffMode);
    mqtt->addDeviceType(&myDHW);
    DHW = &myDHW;
}

void loopHA() {
    mqtt->loop();
}

static void onModeCommand(HAHVAC::Mode mode, HAHVAC * sender) {
    sender->setMode(mode);
}
