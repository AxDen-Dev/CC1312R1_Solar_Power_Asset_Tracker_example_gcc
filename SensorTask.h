

#ifndef SENSORTASK_H_
#define SENSORTASK_H_

#include "Protocol.h"

typedef void (*PacketSendRequestCallback)(uint8_t *payload,
                                          uint8_t payload_size);

void SensorTask_registerPacketSendRequestCallback(
        PacketSendRequestCallback callback);

void SensorTask_init(void);

#endif /* SENSORTASK_H_ */
