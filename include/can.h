#ifndef INCLUDE_CAN_H_
#define INCLUDE_CAN_H_

#include <stdint.h>
#include <config.h>
#include <stddef.h>

struct messsage {
    uint32_t timestamp;
    uint16_t id;
    uint16_t padding;
    uint8_t data[8];
};

#define CAN_MESSAGE_INIT {      \
    .timestamp = 0,             \
    .id = 0,                    \
    .padding = 0,               \
    .data = {[0 ... 7] = 0x00}, \
}

void canInit();
void* canGetSubbufferBlocking(size_t* size);
void canFlushSubbuffer(void* subbuf);

#endif
