#include <assert.h>
#include <daq/buffer.h>

int main() {
    struct buffer buffer = BUFFER_INIT;
    assert(buffer.pos == 0);
    assert(buffer.full == 0);

    return 0;
}
