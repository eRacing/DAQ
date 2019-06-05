#include <assert.h>

#include "message.h"

int main() {
    struct msg_rb ringbuffer = msg_rb_init;
    assert(ringbuffer.writer == 0);
    assert(ringbuffer.reader == 0);
    assert(ringbuffer.loss == 0);

    return 0;
}
