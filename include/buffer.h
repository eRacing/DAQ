#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#define ENTRY_SIZE 18
#define ENTRY_COUNT 128
#define BUFFER_COUNT 8

#include <stdint.h>

struct buffer_entry {
    uint8_t data[ENTRY_SIZE];
};

#define BUFFER_ENTRY_INIT {             \
    .data = {[0 ... ENTRY_SIZE-1] = 0}, \
}

struct buffer {
    struct buffer_entry entries[ENTRY_COUNT];
    uint8_t pos;
    uint8_t full;
};

#define BUFFER_INIT {                                       \
    .entries = {[0 ... ENTRY_COUNT-1] = BUFFER_ENTRY_INIT}, \
    .pos     = 0,                                           \
    .full    = 0,                                           \
}

uint8_t buffer_insert(struct buffer* buffer, struct buffer_entry* entry);

void buffer_flush(struct buffer* buffer);

struct circular {
    struct buffer buffers[BUFFER_COUNT];
    uint8_t writer;
    uint8_t reader;
    uint32_t loss;
};

#define CIRCULAR_INIT {                                 \
    .buffers  = {[0 ... BUFFER_COUNT-1] = BUFFER_INIT}, \
    .writer   = 0,                                      \
    .reader   = 0,                                      \
    .loss     = 0,                                      \
}

void circular_insert(struct circular* circular, struct buffer_entry* entry);

struct buffer* circular_get_buffer_blocking(struct circular* circular);

#endif
