#include <Arduino.h>
#include <ArduinoHA.h>
#include "sensors.h"
#include <OpenTherm.h>

extern OpenTherm ot;

// It is required that OpenTherm-compliant devices support the following data items.
// ID Description                   Master                                    Slave
// 0  Master and slave status flags • Must sent message with READ_DATA        • Must respond with READ_ACK
//                                  • Must support all bits in master status  • Must support all bits in slave status
//
// HB Master config
// 0: CH enable [ CH is disabled, CH is enabled]
HASwitch CHEnabled("CHEnabled");
// 1: DHW enable [ DHW is disabled, DHW is enabled]
HASwitch DHWEnabled("DHWEnabled");
// 2: Cooling enable [ Cooling is disabled, Cooling is enabled]
HASwitch CoolingEnabled("CoolingEnabled");
// 3: OTC active [OTC not active, OTC is active]
HASwitch OTCActive("OTCActive");
// 4: CH2 enable [CH2 is disabled, CH2 is enabled]
HASwitch CH2Enabled("CH2Enabled");

// LB: Slave status flag8 bit: description [ clear/0, set/1]
// 0: fault indication [ no fault, fault ]
HASensor FaultIndication("FaultIndication");
// 1: CH mode [CH not active, CH active]
HASensor CHMode("CHMode");
// 2: DHW mode [ DHW not active, DHW active]
HASensor DHWMode("DHWMode");
// 3: Flame status [ flame off, flame on ]
HASensor FlameStatus("FlameStatus");
// 4: Cooling status [ cooling mode not active, cooling mode active ]
HASensor CoolingStatus("CoolingStatus");
// 5: CH2 mode [CH2 not active, CH2 active]
HASensor CH2Mode("CH2Mode");
// 6: diagnostic indication [no diagnostics, diagnostic event]
HASensor DiagnosticIndication("DiagnosticIndication");

// 1 Control setpoint               • Must sent message with                  • Must respond with WRITE_ACK
//   ie CH water temp. setpoint       WRITE_DATA or INVALID_DATA
//                                    (not recommended)
// HASensor ControlSetpoint("ControlSetpoint");

// 3 Slave configuration flags      • Must sent message with                  • Must respond with READ_ACK
//                                    READ_DATA (at least at start up)        • Must support all slave configuration
//                                                                              flags
// 0: DHW present [ dhw not present, dhw is present ]
HASensor DHWPresent("DHWPresent");
// 1: Control type [ modulating, on/off ]
HASensor ControlType("ControlType");
// 2: Cooling config [ cooling not supported, cooling supported]
HASensor CoolingConfig("CoolingConfig");
// 3: DHW config [instantaneous or not-specified, storage tank]
HASensor DHWConfig("DHWConfig");
// 4: Master low-off&pump control function [allowed, not allowed]
HASensor PumpControl("PumpControl");
// 5: CH2 present [CH2 not present, CH2 present]
HASensor CH2Present("CH2Present");

// LB: Slave MemberID code u8 0..255 MemberID code of the slave
HASensor SlaveMemeberID("SlaveMemeberID");

// 14 Maximum relative modulation   • Not mandatory                           • Must respond with WRITE_ACK
//    level setting                 • Recommended for use in on-off
//                                    control mode.
//
//
//

// 17 Relative modulation level     • Not mandatory                           • Must respond with READ_ACK or DATA_INVALID
HASensor RelativeModulationLevel("RelativeModulationLevel");

// 25 Boiler temperature            • Not mandatory                           • Must respond with READ_ACK or
//                                                                              DATA_INVALID (for example if
//                                                                              sensor fault)
HASensor BoilerTemperature("BoilerTemperature");

// The slave can respond to all other read/write requests, if not supported, with an UNKNOWN-DATAID reply.
// Master units (typically room controllers) should be designed to act in a manner consistent with this rule.

HASensor Presure("CHPresure");
HASensor PresureV("CHPresureV");

void onSwitchCommand(bool state, HASwitch *sender)
{
    sender->setState(state); // report state back to the Home Assistant
}

void initConfig(const HADevice &d)
{
    // HB Master config
    CHEnabled.setName("CH enable");
    CHEnabled.setRetain(true);
    CHEnabled.onCommand(onSwitchCommand);

    DHWEnabled.setName("DHW enable");
    DHWEnabled.setRetain(true);
    DHWEnabled.onCommand(onSwitchCommand);

    CoolingEnabled.setName("Cooling enable");
    CoolingEnabled.setRetain(true);
    CoolingEnabled.onCommand(onSwitchCommand);

    OTCActive.setName("OTC active");
    OTCActive.setRetain(true);
    OTCActive.onCommand(onSwitchCommand);

    CH2Enabled.setName("CH2 enable");
    CH2Enabled.setRetain(true);
    CH2Enabled.onCommand(onSwitchCommand);

    // LB Slave status
    FaultIndication.setName("Fault Indication");
    CHMode.setName("CH Mode");
    DHWMode.setName("DHW Mode");
    FlameStatus.setName("Flame status");
    CoolingStatus.setName("Cooling status");
    CH2Mode.setName("CH2 mode");
    DiagnosticIndication.setName("Diagnostic indication");

    DHWPresent.setName("DHW present");
    ControlType.setName("Control type");
    CoolingConfig.setName("Cooling config");
    DHWConfig.setName("DHW config");
    PumpControl.setName("Master low-off&pump control function");
    CH2Present.setName("CH2 present");
    SlaveMemeberID.setName("Slave MemberID");

    // 17 Relative modulation level     • Not mandatory                           • Must respond with READ_ACK or DATA_INVALID
    RelativeModulationLevel.setName("Relative modulation level");
    RelativeModulationLevel.setUnitOfMeasurement("%");

    // 25 Boiler temperature            • Not mandatory                           • Must respond with READ_ACK or
    //                                                                              DATA_INVALID (for example if
    //                                                                              sensor fault)
    BoilerTemperature.setName("Boiler temperature");
    BoilerTemperature.setDeviceClass("temperature");
    BoilerTemperature.setUnitOfMeasurement("C");

    Presure.setName("CH Presure");
    Presure.setDeviceClass("pressure");
    Presure.setUnitOfMeasurement("bar");
    PresureV.setName("CH Presure V");
    PresureV.setDeviceClass("pressure");
    PresureV.setUnitOfMeasurement("V");
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

    // 0 Config
    uint16_t config = 0;
    // HB
    config |= (CHEnabled.getCurrentState() ? BIT_CHENABLED : 0);
    config |= (DHWEnabled.getCurrentState() ? BIT_DHWENABLED : 0);
    config |= (CoolingEnabled.getCurrentState() ? BIT_COOLINGENABLED : 0);
    config |= (OTCActive.getCurrentState() ? BIT_OTCACTIVE : 0);
    config |= (CH2Enabled.getCurrentState() ? BIT_CH2ENABLED : 0);
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

    request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::SConfigSMemberIDcode, 0);
    response = ot.sendRequest(request);
    DEBUG("Response SConfig %08x\n", response);

    if (ot.isValidResponse(response))
    {
        // 0: DHW present [ dhw not present, dhw is present ]
        DHWPresent.setValue((response & BIT_DHWPRESENT) == BIT_DHWPRESENT ? "dhw is present" : "dhw not present");
        // 1: Control type [ modulating, on/off ]
        ControlType.setValue((response & BIT_CONTROLTYPE) == BIT_CONTROLTYPE ? "on/off" : "modulating");
        // 2: Cooling config [ cooling not supported, cooling supported]
        CoolingConfig.setValue((response & BIT_COOLINGCONFIG) == BIT_COOLINGCONFIG ? "cooling supported" : "cooling not supported");
        // 3: DHW config [instantaneous or not-specified, storage tank]
        DHWConfig.setValue((response & BIT_DHWCONFIG) == BIT_DHWCONFIG ? "storage tank" : "instantaneous or not-specified");
        // 4: Master low-off&pump control function [allowed, not allowed]
        PumpControl.setValue((response & BIT_PUMPCONTROL) == BIT_PUMPCONTROL ? "not allowed" : "allowed");
        // 5: CH2 present [CH2 not present, CH2 present]
        CH2Present.setValue((response & BIT_CH2PRESENT) == BIT_CH2PRESENT ? "CH2 present" : "CH2 not present");
        char buffer[10];
        sprintf(buffer, "%u", response & 0xFF);
        SlaveMemeberID.setValue(buffer);
    }
    else
    {
        DHWPresent.setValue("INVALID");
        ControlType.setValue("INVALID");
        CoolingConfig.setValue("INVALID");
        DHWConfig.setValue("INVALID");
        PumpControl.setValue("INVALID");
        CH2Present.setValue("INVALID");
        SlaveMemeberID.setValue("INVALID");
    }

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

    request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::Tboiler, 0);
    response = ot.sendRequest(request);
    DEBUG("Response %08x\n", response);
    if (ot.isValidResponse(response))
    {
        char buffer[10];
        sprintf(buffer, "%2f", ot.getFloat(response));
        BoilerTemperature.setValue(buffer);
    }
    else
    {
        BoilerTemperature.setValue("INVALID");
    }

    request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::OpenThermVersionSlave, 0);
    response = ot.sendRequest(request);
    DEBUG("Response %08x\n", response);

    if (ot.isValidResponse(response))
    {
        DEBUG("Slave openthem version %f", ot.getFloat(response));
    }
    else
    {
        DEBUG("Error\n");
    }

    double pressureV = readPresure();
    double pressure = map(pressureV, 0.33, 3.3, 0, 12) - 0.4;

    DEBUG("Pressure (bar): %.5f (%.5f V)\n", pressure, pressureV);

    char buffer[10];
    sprintf(buffer, "%.2f", pressure);
    Presure.setValue(buffer);
    sprintf(buffer, "%.2f", pressureV);
    PresureV.setValue(buffer);

    lastCall = millis();
};

double map(double x, double in_min, double in_max, double out_min, double out_max) {
    const double dividend = out_max - out_min;
    const double divisor = in_max - in_min;
    const double delta = x - in_min;

    return (delta * dividend + (divisor / 2.0)) / divisor + out_min;
};

float readPresure()
{
    int count = 0;
    double av;
    while (count < 10)
    {
        auto c = (analogRead(A0) * 3.3) / 1023.0;
        av = (av + c) / 2;
        count++;
    }

    return av;
};
