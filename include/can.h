#ifndef INCLUDE_CAN_H_
#define INCLUDE_CAN_H_

#include <stdint.h>
#include <config.h>
#include <stddef.h>

struct messsage {
    uint32_t timestamp;
    uint16_t id;
    uint8_t data[8];
};

void canInit();
void* canGetSubbufferBlocking(size_t* size);
void canFlushSubbuffer(void* subbuf);

#endif
