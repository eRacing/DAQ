#ifndef INCLUDE_OBJECT_H_
#define INCLUDE_OBJECT_H_

#include <ringbuffer.h>

#define MSG_RB_SUBBUF_SIZE 10
#define MSG_RB_SUBBUF_COUNT 20

struct msg {
    uint32_t timestamp;
    uint8_t data[8];
};

RINGBUFFER_DECLARE(msg, struct msg, MSG_RB_SUBBUF_SIZE, MSG_RB_SUBBUF_COUNT)

#endif
