#ifndef INCLUDE_CAN_H_
#define INCLUDE_CAN_H_

#include <stdint.h>
#include <ringbuffer.h>
#include <config.h>

struct messsage {
    uint32_t timestamp;
    uint16_t id;
    uint8_t data[8];
};

RINGBUFFER_DECLARE(
    can,
    struct messsage,
    CAN_RINGBUFFER_SUBBUF_SIZE,
    CAN_RINGBUFFER_SUBBUF_COUNT
)

void canInit();

#endif
