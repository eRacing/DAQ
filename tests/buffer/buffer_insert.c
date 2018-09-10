#include <assert.h>
#include <daq/buffer.h>

int main() {
    struct buffer buffer = BUFFER_INIT;
    struct buffer_entry entry = BUFFER_ENTRY_INIT;
    for(int i = 0; i < ENTRY_SIZE; i++) entry.data[i] = i;

    /* simple insert */
    assert(buffer_insert(&buffer, &entry) == 0);
    assert(buffer.pos == 1);
    assert(buffer.full == 0);
    for(int i = 0; i < ENTRY_SIZE; i++)
        assert(buffer.entries[0].data[i] == i);

    /* full the buffer until the last entry */
    for(int i = 0; i < ENTRY_COUNT-2; i++) {
        assert(buffer_insert(&buffer, &entry) == 0);
        for(int j = 0; j < ENTRY_SIZE; j++)
            assert(buffer.entries[i].data[j] == j);
    }
    assert(buffer.pos == ENTRY_COUNT-1);
    assert(buffer.full == 0);

    /* add the last entry */
    assert(buffer_insert(&buffer, &entry) == 1);
    for(int i = 0; i < ENTRY_SIZE; i++)
        assert(buffer.entries[ENTRY_COUNT-1].data[i] == i);
    assert(buffer.pos == ENTRY_COUNT);
    assert(buffer.full == 1);

    /* try to add another entry */
    assert(buffer_insert(&buffer, &entry) == 1);
    assert(buffer.pos == ENTRY_COUNT);
    assert(buffer.full == 1);

    return 0;
}
