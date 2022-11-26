// void makeString(const char *data, const int length, char *output)
// {
//     for (int i = 0; i < length; i++)
//     {
//         output[i] = data[i];
//     }
//     output[length] = 0;
// };

// void proceedOT(int, GParser &);

// const char *CMD_TOPIC = "esphome/baxislim/diag/cmd";

// void onMyMessage(const char *topic, const uint8_t *payload, uint16_t length)
// {
//     if (strncmp(topic, CMD_TOPIC, 40) == 0)
//     {
//         char buffer[length + 1];
//         makeString((char *)payload, length, buffer);

//         GParser parser(buffer, ' ');
//         int count = parser.split();
//         if (count > 0)
//         {
//             char *cmd = parser[0];
//             if (strncmp(cmd, "OT", 2) == 0)
//             {
//                 proceedOT(count, parser);
//                 return;
//             }

//             Serial.print("Unknown cmd\n");
//         }
//     }
// }

// unsigned long buildRequest(OpenThermMessageType type, uint8_t id, unsigned int data)
// {
//     unsigned long request = data;
//     if (type == OpenThermMessageType::WRITE_DATA)
//     {
//         request |= 1ul << 28;
//     }
//     request |= ((unsigned long)id) << 16;
//     if (ot.parity(request))
//         request |= (1ul << 31);
//     return request;
// };

// const char *CMD_RES = "esphome/baxislim/diag/res";

// void proceedOT(int count, GParser &parser)
// {
//     // 0  1   2  3         4
//     // OT R|W ID [payload] [fmt=flag8|u8|s8|f8.8|u16|s16]
//     if (count < 3)
//     {
//         Serial.print("To less args\n");
//         return;
//     }

//     OpenThermMessageType type = strncmp(parser[1], "W", 1) == 0
//                                     ? OpenThermMessageType::WRITE
//                                     : OpenThermMessageType::READ;

//     uint8_t dataId = parser.getInt(2);
//     uint16_t payload = 0;
//     if (count > 3)
//     {
//         payload = parser.getInt(3);
//     }
//     char *fmt = "hex";
//     if (count > 4)
//     {
//         fmt = parser[4];
//     }

//     uint32_t request = buildRequest(type, dataId, payload);
//     char buffer[100];
//     sprintf(buffer, "REQ: %08x", request);
//     Serial.println(buffer);
//     mqtt.publish(CMD_RES, buffer);

//     uint32_t response = ot.sendRequest(request);

//     if (strncmp(fmt, "flag8", 5)==0)
//     {
//         uint16_t data = ot.getUInt(response);
//         sprintf(buffer, "%02x %02x", data>>8, data & 0xFF);
//     }
//     else if (strncmp(fmt, "u8", 2)==0)
//     {
//         uint16_t data = ot.getUInt(response);
//         sprintf(buffer, "%u %u", data>>8, data & 0xFF);
//     }
//     else if (strncmp(fmt, "s8", 2)==0)
//     {
//         uint16_t data = ot.getUInt(response);
//         sprintf(buffer, "%i %i",  data>>8, data & 0xFF);
//     }
//     else if (strncmp(fmt, "f8.8", 4)==0)
//     {
//         float data = ot.getFloat(response);
//         sprintf(buffer, "%f", data);
//     }
//     else if (strncmp(fmt, "s16", 3)==0)
//     {
//         int16_t data = (int16_t)ot.getUInt(response);
//         sprintf(buffer, "%i", data);
//     }
//     else if (strncmp(fmt, "u16", 3)==0)
//     {
//         uint16_t data = ot.getUInt(response);
//         sprintf(buffer, "%u", data);
//     }
//     else
//     { // hex
//         sprintf(buffer, "%08x", response);
//     }

//     // send response
//     char buffer2[200];
//     const char * status = ot.statusToString(ot.getLastResponseStatus());
//     const char * msgType = ot.messageTypeToString(ot.getMessageType(response));

//     sprintf(buffer2, "%s: %s %s", status, msgType, buffer);
//     Serial.println(buffer2);
//     mqtt.publish(CMD_RES, buffer2);
// }