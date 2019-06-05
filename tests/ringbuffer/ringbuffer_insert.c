#include <assert.h>

#include "message.h"

#define LOSS_PACKETS 1000

int main() {
    struct msg_rb ringbuffer = msg_rb_init;
    struct msg msg = {
        .timestamp = 1337,
        .data = {0, 1, 2, 3, 4, 5, 6, 7},
    };

    /* simple insert */
    msg_rb_insert(&ringbuffer, &msg);
    assert(ringbuffer.writer == 0);
    assert(ringbuffer.reader == 0);
    assert(ringbuffer.loss == 0);

    /* fill the first buffer */
    for(int i = 0; i < MSG_RB_SUBBUF_SIZE-1; i++)
        msg_rb_insert(&ringbuffer, &msg);
    assert(ringbuffer.writer == 1);
    assert(ringbuffer.reader == 0);
    assert(ringbuffer.loss == 0);

    /* fill all buffer until the last */
    for(int i = 0; i < MSG_RB_SUBBUF_COUNT-2; i++)
        for(int j = 0; j < MSG_RB_SUBBUF_SIZE; j++)
            msg_rb_insert(&ringbuffer, &msg);
    assert(ringbuffer.writer == MSG_RB_SUBBUF_COUNT-1);
    assert(ringbuffer.reader == 0);
    assert(ringbuffer.loss == 0);

    /* fill the last buffer, it'll loop back */
    for(int j = 0; j < MSG_RB_SUBBUF_SIZE; j++)
        msg_rb_insert(&ringbuffer, &msg);
    assert(ringbuffer.writer == 0);
    assert(ringbuffer.reader == 0);
    assert(ringbuffer.loss == 0);

    /* next insert will be loss */
    for(int i = 0; i < LOSS_PACKETS; i++)
	msg_rb_insert(&ringbuffer, &msg);
    assert(ringbuffer.writer == 0);
    assert(ringbuffer.reader == 0);
    assert(ringbuffer.loss == LOSS_PACKETS);


    /* free the first buffer */
    struct msg_rb_subbuf* read = msg_rb_get_subbuf(&ringbuffer);
    msg_rb_subbuf_flush(read);
    assert(read == &(ringbuffer.buffers[0]));
    assert(ringbuffer.writer == 0);
    assert(ringbuffer.reader == 1);
    assert(ringbuffer.loss == LOSS_PACKETS);

    /* fill the first buffer again */
    for(int i = 0; i < MSG_RB_SUBBUF_SIZE; i++)
        msg_rb_insert(&ringbuffer, &msg);
    assert(ringbuffer.writer == 1);
    assert(ringbuffer.reader == 1);
    assert(ringbuffer.loss == LOSS_PACKETS);

    /* next insert will be loss */
    for(int i = 0; i < LOSS_PACKETS; i++) msg_rb_insert(&ringbuffer, &msg);
    assert(ringbuffer.writer == 1);
    assert(ringbuffer.reader == 1);
    assert(ringbuffer.loss == 2*LOSS_PACKETS);

    return 0;
}
