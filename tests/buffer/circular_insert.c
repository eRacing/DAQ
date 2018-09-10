#include <assert.h>
#include <daq/buffer.h>

#define LOSS_PACKETS 1000

int main() {
    struct circular circular = CIRCULAR_INIT;
    struct buffer_entry entry = BUFFER_ENTRY_INIT;
    for(int i = 0; i < ENTRY_SIZE; i++) entry.data[i] = i;

    /* simple insert */
    circular_insert(&circular, &entry);
    assert(circular.writer == 0);
    assert(circular.reader == 0);
    assert(circular.loss == 0);
    
    /* fill the first buffer */
    for(int i = 0; i < ENTRY_COUNT-1; i++)
        circular_insert(&circular, &entry);
    assert(circular.writer == 1);
    assert(circular.reader == 0);
    assert(circular.loss == 0);

    /* fill all buffer until the last */
    for(int i = 0; i < BUFFER_COUNT-2; i++)
        for(int j = 0; j < ENTRY_COUNT; j++)
            circular_insert(&circular, &entry);
    assert(circular.writer == BUFFER_COUNT-1);
    assert(circular.reader == 0);
    assert(circular.loss == 0);

    /* fill the last buffer, it'll loop back */
    for(int j = 0; j < ENTRY_COUNT; j++)
        circular_insert(&circular, &entry);
    assert(circular.writer == 0);
    assert(circular.reader == 0);
    assert(circular.loss == 0);

    /* next insert will be loss */
    for(int i = 0; i < LOSS_PACKETS; i++) circular_insert(&circular, &entry);
    assert(circular.writer == 0);
    assert(circular.reader == 0);
    assert(circular.loss == LOSS_PACKETS);

    /* free the first buffer */
    struct buffer* read = circular_get_buffer_blocking(&circular);
    buffer_flush(read);
    assert(read == &(circular.buffers[0]));
    assert(circular.writer == 0);
    assert(circular.reader == 1);
    assert(circular.loss == LOSS_PACKETS);

    /* fill the first buffer again */
    for(int i = 0; i < ENTRY_COUNT; i++)
        circular_insert(&circular, &entry);
    assert(circular.writer == 1);
    assert(circular.reader == 1);
    assert(circular.loss == LOSS_PACKETS);

    /* next insert will be loss */
    for(int i = 0; i < LOSS_PACKETS; i++) circular_insert(&circular, &entry);
    assert(circular.writer == 1);
    assert(circular.reader == 1);
    assert(circular.loss == 2*LOSS_PACKETS);

    return 0;
}
