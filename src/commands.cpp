#include <Arduino.h>
#include <string.h>
#include <Shell.h>

#include "ot.h"

ShellCommand(OT, "Send OT command ex: ot r|w <id> [[<data>] [flag8|u8|s8|f8.8|u16|s16]]",
             [](Shell &shell, ShellCommandRegister *command, int argc, const ShellArguments &argv)
             {
                 if (argc < 3)
                 {
                     shell.print("To less args\n");
                     return;
                 }

                 OpenThermMessageType type = (strncmp(argv[1], "W", 1) == 0 || strncmp(argv[1], "w", 1) == 0)
                                                 ? OpenThermMessageType::WRITE
                                                 : OpenThermMessageType::READ;

                 uint8_t dataId = atoi(argv[2]);
                 uint16_t payload = 0;
                 if (argc > 3)
                 {
                     payload = atoi(argv[3]);
                 }

                 char fmt[10] = "hex";
                 if (argc > 4)
                 {
                     strncpy(fmt, argv[4], sizeof(fmt)-1);
                 }

                 uint32_t request = otBuildRequest(type, dataId, payload);
                 char buffer[100];
                 sprintf(buffer, "REQ: %08x", request);
                 shell.println(buffer);

                 uint32_t response = ot->sendRequest(request);

                 if (strncmp(fmt, "flag8", 5) == 0)
                 {
                     uint16_t data = ot->getUInt(response);
                     sprintf(buffer, "%02x %02x", data >> 8, data & 0xFF);
                 }
                 else if (strncmp(fmt, "u8", 2) == 0)
                 {
                     uint16_t data = ot->getUInt(response);
                     sprintf(buffer, "%u %u", data >> 8, data & 0xFF);
                 }
                 else if (strncmp(fmt, "s8", 2) == 0)
                 {
                     uint16_t data = ot->getUInt(response);
                     sprintf(buffer, "%i %i", data >> 8, data & 0xFF);
                 }
                 else if (strncmp(fmt, "f8.8", 4) == 0)
                 {
                     float data = ot->getFloat(response);
                     sprintf(buffer, "%f", data);
                 }
                 else if (strncmp(fmt, "s16", 3) == 0)
                 {
                     int16_t data = (int16_t)ot->getUInt(response);
                     sprintf(buffer, "%i", data);
                 }
                 else if (strncmp(fmt, "u16", 3) == 0)
                 {
                     uint16_t data = ot->getUInt(response);
                     sprintf(buffer, "%u", data);
                 }
                 else
                 { // hex
                     sprintf(buffer, "%08x", response);
                 }

                 // send response
                 char buffer2[200];
                 const char *status = ot->statusToString(ot->getLastResponseStatus());
                 const char *msgType = ot->messageTypeToString(ot->getMessageType(response));

                 sprintf(buffer2, "%s: %s %s", status, msgType, buffer);
                 shell.println(buffer2);
             });
