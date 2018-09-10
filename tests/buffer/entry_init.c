#include <assert.h>
#include <daq/buffer.h>

int main() {
    struct buffer_entry entry = BUFFER_ENTRY_INIT;
    for(int i = 0; i < ENTRY_SIZE; i++) assert(entry.data[i] == 0);

    return 0;
}
