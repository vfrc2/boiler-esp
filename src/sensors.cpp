#include <Arduino.h>
#include <ArduinoHA.h>
#include <OpenTherm.h>

#include "sensors.h"
#include "config.h"

extern OpenTherm ot;

#define BIT_CHENABLED (1 << 8)
#define BIT_DHWENABLED (1 << 9)
#define BIT_COOLINGENABLED (1 << 10)
#define BIT_OTCACTIVE (1 << 11)
#define BIT_CH2ENABLED (1 << 12)
#define BIT_FAULTINDICATOR (1)
#define BIT_CHMODE (1 << 1)
#define BIT_DHWMODE (1 << 2)
#define BIT_FLAMESTATUS (1 << 3)
#define BIT_COOLINGSTATUS (1 << 4)
#define BIT_CH2MODE (1 << 5)
#define BIT_DIAGNOSTICINDICATION (1 << 6)
#define BIT_DHWPRESENT (1 << 8)
#define BIT_CONTROLTYPE (1 << 9)
#define BIT_COOLINGCONFIG (1 << 10)
#define BIT_DHWCONFIG (1 << 11)
#define BIT_PUMPCONTROL (1 << 12)
#define BIT_CH2PRESENT (1 << 13)

HASensor FaultIndication("FaultIndication");
HASensor CHMode("CHMode");
HASensor DHWMode("DHWMode");
HASensor FlameStatus("FlameStatus");
HASensor CoolingStatus("CoolingStatus");
HASensor CH2Mode("CH2Mode");
HASensor DiagnosticIndication("DiagnosticIndication");

HASensor SlaveConfig("SlaveConfig");
HASensor SlaveMemeberID("SlaveMemeberID");
HASensor RelativeModulationLevel("RelativeModulationLevel");

HASensor BoilerTemperature("BoilerTemperature");

HASensor Presure("CHPresure");
HASensor PresureV("CHPresureV");

HAHVAC CH("CH", HAHVAC::TargetTemperatureFeature | HAHVAC::PowerFeature | HAHVAC::ModesFeature);
HAHVAC DHW("DHW", HAHVAC::TargetTemperatureFeature | HAHVAC::PowerFeature | HAHVAC::ModesFeature);

void onTargetTemperatureCommand(HANumeric temperature, HAHVAC *sender)
{
    float temperatureFloat = temperature.toFloat();

    Serial.print("Target temperature: ");
    Serial.println(temperatureFloat);

    bool result = false;
    if (sender == &CH)
    {
        result = ot.setBoilerTemperature(temperatureFloat);
    }
    else if (sender == &DHW)
    {
        result = ot.setDHWSetpoint(temperatureFloat);
    }

    sender->setTargetTemperature(temperatureFloat); // report target temperature back to the HA panel
    if (result)
    {
        DEBUG("Error set target temperature: ");
    }
}

void onModeCommand(HAHVAC::Mode mode, HAHVAC * sender) {
    Serial.println("Change mode");
    sender->setMode(mode);
}

void onSwitchCommand(bool state, HASwitch *sender)
{
    sender->setState(state); // report state back to the Home Assistant
}

void initConfig(HADevice &d, HAMqtt &mqtt)
{
    // LB Slave status
    FaultIndication.setName("Fault Indication");
    mqtt.addDeviceType(&FaultIndication);
    CHMode.setName("CH Mode");
    mqtt.addDeviceType(&CHMode);
    DHWMode.setName("DHW Mode");
    mqtt.addDeviceType(&DHWMode);
    FlameStatus.setName("Flame status");
    mqtt.addDeviceType(&FlameStatus);
    CoolingStatus.setName("Cooling status");
    mqtt.addDeviceType(&CoolingStatus);
    CH2Mode.setName("CH2 mode");
    mqtt.addDeviceType(&CH2Mode);
    DiagnosticIndication.setName("Diagnostic indication");
    mqtt.addDeviceType(&DiagnosticIndication);

    SlaveConfig.setName("Slave config");
    mqtt.addDeviceType(&SlaveConfig);
    SlaveMemeberID.setName("Slave MemberID");
    mqtt.addDeviceType(&SlaveMemeberID);

    RelativeModulationLevel.setName("Relative modulation level");
    RelativeModulationLevel.setUnitOfMeasurement("%");
    mqtt.addDeviceType(&RelativeModulationLevel);

    Presure.setName("CH Presure");
    Presure.setDeviceClass("pressure");
    Presure.setUnitOfMeasurement("bar");
    mqtt.addDeviceType(&Presure);
    PresureV.setName("CH Presure V");
    PresureV.setDeviceClass("pressure");
    PresureV.setUnitOfMeasurement("V");
    mqtt.addDeviceType(&PresureV);

    CH.setName("CH");
    CH.onTargetTemperatureCommand(onTargetTemperatureCommand);
    // CH.onPowerCommand(onPowerCommand);
    CH.onModeCommand(onModeCommand);
    CH.setTargetTemperature(21);
    CH.setMaxTemp(80);
    CH.setMinTemp(10);
    CH.setRetain(true);
    CH.setTempStep(0.5);
    CH.setModes(HAHVAC::HeatMode | HAHVAC::OffMode);
    mqtt.addDeviceType(&CH);

    DHW.setName("DHW");
    DHW.onTargetTemperatureCommand(onTargetTemperatureCommand);
    DHW.onModeCommand(onModeCommand);
    DHW.setTargetTemperature(21);
    DHW.setMaxTemp(80);
    DHW.setMinTemp(10);
    DHW.setRetain(true);
    DHW.setTempStep(0.5);
    DHW.setModes(HAHVAC::HeatMode | HAHVAC::OffMode);
    mqtt.addDeviceType(&DHW);
}

unsigned long lastCall = millis();

float readPresure();
double map(double x, double in_min, double in_max, double out_min, double out_max);

void readLoop()
{
    auto now = millis();
    if ((now - lastCall) < 5000 /* 5s */)
    {
        return;
    }

    uint32_t response;
    uint32_t request;

    // Send master config
    DEBUG("Send master config %i\n", OT_MASTER_ID);
    request = ot.buildRequest(OpenThermRequestType::WRITE_DATA, OpenThermMessageID::MConfigMMemberIDcode, OT_MASTER_ID);
    response = ot.sendRequest(request);
    DEBUG("Response %08x\n", response);

    // Read slave config
    DEBUG("Read slave config\n");
    request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::SConfigSMemberIDcode, 0);
    response = ot.sendRequest(request);
    DEBUG("Response Slave config %08x\n", response);

    if (ot.isValidResponse(response))
    {
        char buffer[200];
        sprintf(buffer, "%s, %s, %s, %s, %s, %s (%02x)",
                // 0: DHW present [ dhw not present, dhw is present ]
                (response & BIT_DHWPRESENT) == BIT_DHWPRESENT ? "dhw is present" : "dhw not present",
                // 1: Control type [ modulating, on/off ]
                (response & BIT_CONTROLTYPE) == BIT_CONTROLTYPE ? "Control type: on/off" : "Control type: modulating",
                // 2: Cooling config [ cooling not supported, cooling supported]
                (response & BIT_COOLINGCONFIG) == BIT_COOLINGCONFIG ? "cooling supported" : "cooling not supported",
                // 3: DHW config [instantaneous or not-specified, storage tank]
                (response & BIT_DHWCONFIG) == BIT_DHWCONFIG ? "DHW config is storage tank" : "DHW instantaneous or not-specified",
                // 4: Master low-off&pump control function [allowed, not allowed]
                (response & BIT_PUMPCONTROL) == BIT_PUMPCONTROL ? "Pump control not allowed" : "Pump control allowed",
                // 5: CH2 present [CH2 not present, CH2 present]
                (response & BIT_CH2PRESENT) == BIT_CH2PRESENT ? "CH2 present" : "CH2 not present",
                response);
        SlaveConfig.setValue(buffer);

        if ((response & BIT_DHWPRESENT) == BIT_DHWPRESENT)
        {
            DHW.setModes(HAHVAC::HeatMode | HAHVAC::OffMode);
        }
        else
        {
            DHW.setModes(HAHVAC::OffMode);
        }

        char buffer2[10];
        sprintf(buffer, "%u", response & 0xFF);
        SlaveMemeberID.setValue(buffer2);
    }
    else
    {
        SlaveConfig.setValue("INVALID");
        SlaveMemeberID.setValue("INVALID");
    }

    // 0 Config
    uint16_t config = 0;
    // HB
    config |= (CH.getCurrentMode() == HAHVAC::HeatMode ? BIT_CHENABLED : 0);
    config |= (DHW.getCurrentMode() == HAHVAC::HeatMode ? BIT_DHWENABLED : 0);
    config |= (0 ? BIT_COOLINGENABLED : 0);
    config |= (0 ? BIT_OTCACTIVE : 0);
    config |= (0 ? BIT_CH2ENABLED : 0);
    // reserved
    // LB = 0
    DEBUG("Config %08x\n", config);
    request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::Status, config);
    response = ot.sendRequest(request);
    DEBUG("Response %08x\n", response);

    if (ot.isValidResponse(response))
    {
        FaultIndication.setValue((response & BIT_FAULTINDICATOR) == BIT_FAULTINDICATOR ? "fault" : " no fault");
        CHMode.setValue((response & BIT_CHMODE) == BIT_CHMODE ? "CH active" : "CH not active");
        DHWMode.setValue((response & BIT_DHWMODE) == BIT_DHWMODE ? "DHW active" : "DHW not active");
        FlameStatus.setValue((response & BIT_FLAMESTATUS) == BIT_FLAMESTATUS ? "flame on" : "flame off");
        CoolingStatus.setValue((response & BIT_COOLINGSTATUS) == BIT_COOLINGSTATUS ? "cooling mode active" : "cooling mode not active");
        CH2Mode.setValue((response & BIT_CH2MODE) == BIT_CH2MODE ? "CH2 active" : "CH2 not active");
        DiagnosticIndication.setValue((response & BIT_DIAGNOSTICINDICATION) == BIT_DIAGNOSTICINDICATION ? "diagnostic event" : "no diagnostics");
    }
    else
    {
        FaultIndication.setValue("INVALID");
        CHMode.setValue("INVALID");
        DHWMode.setValue("INVALID");
        FlameStatus.setValue("INVALID");
        CoolingStatus.setValue("INVALID");
        CH2Mode.setValue("INVALID");
        DiagnosticIndication.setValue("INVALID");
    }

    DEBUG("Read rel mode level\n");
    request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::RelModLevel, 0);
    response = ot.sendRequest(request);
    DEBUG("Response %08x\n", response);
    if (ot.isValidResponse(response))
    {
        char buffer[10];
        sprintf(buffer, "%2f", ot.getFloat(response));
        RelativeModulationLevel.setValue(buffer);
    }
    else
    {
        RelativeModulationLevel.setValue("INVALID");
    }

    DEBUG("Request CH temp\n");
    request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::Tboiler, 0);
    response = ot.sendRequest(request);
    DEBUG("Response %08x\n", response);
    if (ot.isValidResponse(response))
    {
        CH.setCurrentTemperature(ot.getFloat(response));
    }
    else
    {
        // BoilerTemperature.setValue("INVALID");
    }

    DEBUG("Request DHW temp\n");
    request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::Tdhw, 0);
    response = ot.sendRequest(request);
    DEBUG("Response %08x\n", response);
    if (ot.isValidResponse(response))
    {
        float temp = ot.getFloat(response);
        DHW.setCurrentTemperature(temp);
    }
    else
    {
        // DHW.setCurrentTemperature().setValue("INVALID");
    }

    DEBUG("Request boiler MAX/MIN temp\n");
    request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::MaxTSetUBMaxTSetLB, 0);
    response = ot.sendRequest(request);
    DEBUG("Response %08x\n", response);
    if (ot.isValidResponse(response))
    {
        float maxTemp = static_cast<float>(ot.getUInt(response) >> 8);
        float minTemp = static_cast<float>(ot.getUInt(response) & 0xFF);

        CH.setMaxTemp(maxTemp);
        CH.setMinTemp(minTemp);
    }
    else
    {
        DEBUG("Error read min/max for CH");
        // DHW.setCurrentTemperature().setValue("INVALID");
    }

    DEBUG("Request boiler MAX/MIN temp\n");
    request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::TdhwSetUBTdhwSetLB, 0);
    response = ot.sendRequest(request);
    DEBUG("Response %08x\n", response);
    if (ot.isValidResponse(response))
    {
        float maxTemp = static_cast<float>(ot.getUInt(response) >> 8);
        float minTemp = static_cast<float>(ot.getUInt(response) & 0xFF);

        DHW.setMaxTemp(maxTemp);
        DHW.setMinTemp(minTemp);
    }
    else
    {
        DEBUG("Error read min/max for CH");
        // DHW.setCurrentTemperature().setValue("INVALID");
    }

    double pressureV = readPresure();
    double pressure = map(pressureV, 0.33, 3.3, 0, 12) - 0.4;

    DEBUG("Pressure (bar): %.5f (%.5f V)\n", pressure, pressureV);

    char buffer[10];
    if (pressure > 0)
    {
        sprintf(buffer, "%.2f", pressure);
        Presure.setValue(buffer);
    }
    else
    {
        Presure.setValue("INVALID");
    }
    sprintf(buffer, "%.2f", pressureV);
    PresureV.setValue(buffer);

    lastCall = millis();
};

double map(double x, double in_min, double in_max, double out_min, double out_max)
{
    const double dividend = out_max - out_min;
    const double divisor = in_max - in_min;
    const double delta = x - in_min;

    return (delta * dividend + (divisor / 2.0)) / divisor + out_min;
};

float readPresure()
{
    int count = 0;
    double av = (analogRead(A0) * 3.3) / 1023.0;
    while (count < 10)
    {
        auto c = (analogRead(A0) * 3.3) / 1023.0;
        av = (av + c) / 2;
        count++;
    }

    return av;
};
