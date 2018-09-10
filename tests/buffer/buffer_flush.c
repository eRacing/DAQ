#include <assert.h>
#include <daq/buffer.h>

int main() {
    struct buffer buffer = BUFFER_INIT;
    struct buffer_entry entry = BUFFER_ENTRY_INIT;
    for(int i = 0; i < ENTRY_SIZE; i++) entry.data[i] = i;

    /* fill the buffer */
    for(int i = 0; i < ENTRY_COUNT; i++)
        assert(buffer_insert(&buffer, &entry) == (i == ENTRY_COUNT-1 ? 1 : 0));
    assert(buffer.pos == ENTRY_COUNT);
    assert(buffer.full == 1);

    /* flush the buffer */
    buffer_flush(&buffer);
    assert(buffer.pos == 0);
    assert(buffer.full == 0);

    return 0;
}
