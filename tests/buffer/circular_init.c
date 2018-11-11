#include <assert.h>
#include <buffer.h>

int main() {
    struct circular circular = CIRCULAR_INIT;
    assert(circular.writer == 0);
    assert(circular.reader == 0);
    assert(circular.loss == 0);

    return 0;
}
