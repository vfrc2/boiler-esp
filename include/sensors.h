#pragma once 

#include <ArduinoHA.h>

// It is required that OpenTherm-compliant devices support the following data items.
// ID Description                   Master                                    Slave
// 0  Master and slave status flags • Must sent message with READ_DATA        • Must respond with READ_ACK
//                                  • Must support all bits in master status  • Must support all bits in slave status
//
// HB Master config
// 0: CH enable [ CH is disabled, CH is enabled]
#define BIT_CHENABLED (1<<8)
extern HASwitch CHEnabled;
// 1: DHW enable [ DHW is disabled, DHW is enabled]
#define BIT_DHWENABLED (1<<9)
extern HASwitch DHWEnabled;
// 2: Cooling enable [ Cooling is disabled, Cooling is enabled]
#define BIT_COOLINGENABLED (1<<10)
extern HASwitch CoolingEnabled;
// 3: OTC active [OTC not active, OTC is active]
#define BIT_OTCACTIVE (1<<11)
extern HASwitch OTCActive;
// 4: CH2 enable [CH2 is disabled, CH2 is enabled]
#define BIT_CH2ENABLED (1<<12)
extern HASwitch CH2Enabled;

// LB: Slave status flag8 bit: description [ clear/0, set/1]
// 0: fault indication [ no fault, fault ]
#define BIT_FAULTINDICATOR (1)
extern HASensor FaultIndication;
// 1: CH mode [CH not active, CH active]
#define BIT_CHMODE (1<<1)
extern HASensor CHMode;
// 2: DHW mode [ DHW not active, DHW active]
#define BIT_DHWMODE (1<<2)
extern HASensor DHWMode;
// 3: Flame status [ flame off, flame on ]
#define BIT_FLAMESTATUS (1<<3)
extern HASensor FlamEstatus;
// 4: Cooling status [ cooling mode not active, cooling mode active ]
#define BIT_COOLINGSTATUS (1<<4)
extern HASensor CoolingStatus;
// 5: CH2 mode [CH2 not active, CH2 active]
#define BIT_CH2MODE (1<<5)
extern HASensor CH2Mode;
// 6: diagnostic indication [no diagnostics, diagnostic event]
#define BIT_DIAGNOSTICINDICATION (1<<6)
extern HASensor DiagnosticIndication;

// 1 Control setpoint               • Must sent message with                  • Must respond with WRITE_ACK
//   ie CH water temp. setpoint       WRITE_DATA or INVALID_DATA
//                                    (not recommended)
// extern HASensor ControlSetpoint;

// 3 Slave configuration flags      • Must sent message with                  • Must respond with READ_ACK
//                                    READ_DATA (at least at start up)        • Must support all slave configuration
//                                                                              flags
// 0: DHW present [ dhw not present, dhw is present ]
#define BIT_DHWPRESENT (1<<8)
extern HASensor DHWPresent;
// 1: Control type [ modulating, on/off ]
#define BIT_CONTROLTYPE (1<<9)
extern HASensor ControlType;
// 2: Cooling config [ cooling not supported, cooling supported]
#define BIT_COOLINGCONFIG (1<<10)
extern HASensor CoolingConfig;
// 3: DHW config [instantaneous or not-specified, storage tank]
#define BIT_DHWCONFIG (1<<11)
extern HASensor DHWConfig;
// 4: Master low-off&pump control function [allowed, not allowed]
#define BIT_PUMPCONTROL (1<<12)
extern HASensor PumpControl;
// 5: CH2 present [CH2 not present, CH2 present]
#define BIT_CH2PRESENT (1<<13)
extern HASensor CH2Present;

// LB: Slave MemberID code u8 0..255 MemberID code of the slave
extern HASensor SlaveMemeberID;

// 14 Maximum relative modulation   • Not mandatory                           • Must respond with WRITE_ACK
//    level setting                 • Recommended for use in on-off
//                                    control mode.
//
//
//

// 17 Relative modulation level     • Not mandatory                           • Must respond with READ_ACK or DATA_INVALID
extern HASensor RelativeModulationLevel;

// 25 Boiler temperature            • Not mandatory                           • Must respond with READ_ACK or
//                                                                              DATA_INVALID (for example if
//                                                                              sensor fault)
extern HASensor BoilerTemperature;

// The slave can respond to all other read/write requests, if not supported, with an UNKNOWN-DATAID reply.
// Master units (typically room controllers) should be designed to act in a manner consistent with this rule.

extern HASensor Presure;

extern HAHVAC HVAC;

void initConfig(HADevice & device, HAMqtt & mqtt);

void readLoop();