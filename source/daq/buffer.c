#include <string.h>
#include <daq/buffer.h>

uint8_t buffer_insert(struct buffer* buffer, struct buffer_entry* entry) {
    /* make sure the buffer isn't full */
    if(buffer->full) return 1;

    /* increase the next position to write */
    uint8_t pos = buffer->pos++;

    /* copy the entry */
    memcpy(&(buffer->entries[pos]), entry, sizeof(struct buffer_entry));

    /* check if the buffer is now full */
    if(buffer->pos >= ENTRY_COUNT) buffer->full = 1;

    return buffer->full;
}

void buffer_flush(struct buffer* buffer) {
    buffer->full = 0;
    buffer->pos = 0;
}

void circular_insert(struct circular* circular, struct buffer_entry* entry) {
    /* get the writer's position */
    uint8_t writer = circular->writer;

    /* get the buffer to write the entry into */
    struct buffer* buffer = &(circular->buffers[writer]);

    /* make sure it isn't full */
    if(buffer->full) {
        circular->loss++;
        return;
    }

    /* insert the entry into the buffer */
    uint8_t full = buffer_insert(buffer, entry);

    /* write to the next buffer it is full */
    if(full) circular->writer = (writer + 1) % BUFFER_COUNT;
}

struct buffer* circular_get_buffer_blocking(struct circular* circular) {
    /* get the next buffer to flush */
    struct buffer* buffer = &(circular->buffers[circular->reader++]);
    circular->reader %= BUFFER_COUNT;

    /* wait until it is full */
    while(!(buffer->full));

    return buffer;
}
