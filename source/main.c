#include "globals_lib.h"
#include "timer_lib.h"
#include "can_lib.h"
#include "io_lib.h"
#include "buffer.h"

int main(void){
    FPULazyStackingEnable();

    // Clock initialisations
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); // 80 MHz (400MHz / 2 / 2.5)

    IntMasterEnable();

    InitCan();

    InitTimer();

    InitIo();

    InitFile();

    while(1){
        struct buffer* buffer = circular_get_buffer_blocking((struct circular*) &canmessages);
        writeLogEntries(buffer);
        buffer_flush(buffer);
    }
}
