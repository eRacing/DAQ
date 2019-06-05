#ifndef INCLUDE_RINGBUFFER_H_
#define INCLUDE_RINGBUFFER_H_

/*
 * This file implements a generic, single-writer, single-reader, constant-sized
 * and statically-allocated ring buffer.
 */

#include <stdint.h>
#include <string.h>

#define RINGBUFFER_DECLARE_SUBBUFFER(prefix, entry_type, subbuf_size) \
struct prefix##_rb_subbuf {          \
    entry_type entries[subbuf_size]; \
    unsigned pos;                    \
    uint8_t full;                    \
};

#define RINGBUFFER_DECLARE_STRUCT(prefix, subbuf_count) \
struct prefix##_rb {                                    \
    struct prefix##_rb_subbuf buffers[subbuf_count];    \
    unsigned writer;                                    \
    unsigned reader;                                    \
    unsigned loss;                                      \
};

/* TODO: rename this macro */
#ifndef RINGBUFFER_DECLARE_DATA_TYPE

#define RINGBUFFER_DECLARE_STRUCT_INIT(prefix, subbuf_count) \
extern const struct prefix##_rb prefix##_rb_init;

#define RINGBUFFER_DECLARE_SUBBUFFER_INSERT(prefix, entry_type, subbuf_size)

#define RINGBUFFER_DECLARE_SUBBUFFER_FLUSH(prefix) \
void prefix##_rb_subbuf_flush(struct prefix##_rb_subbuf* subbuf);

#define RINGBUFFER_DECLARE_INSERT(prefix, entry_type, subbuf_count) \
void prefix##_rb_insert(struct prefix##_rb* rb, entry_type* data);

#define RINGBUFFER_DECLARE_GET_SUBBUF(prefix, subbuf_count) \
struct prefix##_rb_subbuf* prefix##_rb_get_subbuf(struct prefix##_rb* rb);

#else /* RINGBUFFER_DECLARE_DATA_TYPE */

#define RINGBUFFER_DECLARE_STRUCT_INIT(prefix, subbuf_count)       \
const struct prefix##_rb prefix##_rb_init = {                      \
    .buffers = {[0 ... subbuf_count - 1] = {.pos = 0, .full = 0}}, \
    .writer = 0,                                                   \
    .reader = 0,                                                   \
    .loss = 0,                                                     \
};

#define RINGBUFFER_DECLARE_SUBBUFFER_INSERT(prefix, entry_type, subbuf_size)             \
static                                                                                   \
uint8_t prefix##_rb_subbuf_insert(struct prefix##_rb_subbuf* subbuf, entry_type* data) { \
    /* make sure the subbuffer isn't full */                                             \
    if(subbuf->full) return 1;                                                           \
                                                                                         \
    /* get the next position to write */                                                 \
    unsigned pos = subbuf->pos++;                                                        \
                                                                                         \
    /* copy the data into the subbuffer */                                               \
    memcpy(&subbuf->entries[pos], data, sizeof(entry_type));                             \
                                                                                         \
    /* check if the subbuffer is now full */                                             \
    if(subbuf->pos >= subbuf_size) subbuf->full = 1;                                     \
                                                                                         \
    return subbuf->full;                                                                 \
}

#define RINGBUFFER_DECLARE_SUBBUFFER_FLUSH(prefix)                 \
void prefix##_rb_subbuf_flush(struct prefix##_rb_subbuf* subbuf) { \
    subbuf->full = 0;                                              \
    subbuf->pos = 0;                                               \
}

#define RINGBUFFER_DECLARE_INSERT(prefix, entry_type, subbuf_count) \
void prefix##_rb_insert(struct prefix##_rb* rb, entry_type* data) { \
    /* get the writer's position */                                 \
    unsigned writer = rb->writer;                                   \
                                                                    \
    /* get the next subbuffer to write */                           \
    struct prefix##_rb_subbuf* subbuf = &rb->buffers[writer];       \
                                                                    \
    /* make sure it isn't full */                                   \
    if(subbuf->full) {                                              \
        rb->loss++;                                                 \
        return;                                                     \
    }                                                               \
                                                                    \
    /* insert the entry into the buffer */                          \
    unsigned full = prefix##_rb_subbuf_insert(subbuf, data);        \
                                                                    \
    /* write to the next subbuffer if the last one is now full */   \
    if(full) rb->writer = (writer + 1) % subbuf_count;              \
}

#define RINGBUFFER_DECLARE_GET_SUBBUF(prefix, subbuf_count)                 \
struct prefix##_rb_subbuf* prefix##_rb_get_subbuf(struct prefix##_rb* rb) { \
    /* get the next buffer to flush */                                      \
    struct prefix##_rb_subbuf* subbuf = &rb->buffers[rb->reader++];         \
    rb->reader %= subbuf_count;                                             \
                                                                            \
    /* wait until the subbuffer is full */                                  \
    while(!(subbuf->full));                                                 \
                                                                            \
    return subbuf;                                                          \
}

#endif /* RINGBUFFER_DECLARE_DATA_TYPE */

#define RINGBUFFER_DECLARE(prefix, entry_type, subbuf_size, subbuf_count) \
    RINGBUFFER_DECLARE_SUBBUFFER(prefix, entry_type, subbuf_size)         \
    RINGBUFFER_DECLARE_STRUCT(prefix, subbuf_count)                       \
    RINGBUFFER_DECLARE_STRUCT_INIT(prefix, subbuf_count)                  \
                                                                          \
    RINGBUFFER_DECLARE_SUBBUFFER_INSERT(prefix, entry_type, subbuf_size)  \
    RINGBUFFER_DECLARE_SUBBUFFER_FLUSH(prefix)                            \
    RINGBUFFER_DECLARE_INSERT(prefix, entry_type, subbuf_count)           \
    RINGBUFFER_DECLARE_GET_SUBBUF(prefix, subbuf_count)

#endif
