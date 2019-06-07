#include <sdcard.h>
#include <can.h>
#include <led.h>
#include <utils.h>
#include <stdbool.h>
#include <stdint.h>

#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>

////////////////
// Public API //
////////////////

void daqInit() {
    /* setup 40MHz clock */
    SysCtlClockSet(SYSCTL_SYSDIV_5 |SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    /* disable interrupts */
    assert(!IntMasterDisable());

    /* initialize LEDs */
    ledInit();

    /* initialize CAN */
    canInit();

    /* initialize SD card */
    sdcardInit();

    /* enable interrupts */
    assert(IntMasterEnable());
}

void daqLoop() {
    while(1) {
        /* TODO: do something useful with the DAQ */
        ledEnable(LED_RED);
        ledDisable(LED_YELLOW);
        ledDisable(LED_GREEN);
        SysCtlDelayMS(500);

        ledDisable(LED_RED);
        ledEnable(LED_YELLOW);
        ledDisable(LED_GREEN);
        SysCtlDelayMS(500);

        ledDisable(LED_RED);
        ledDisable(LED_YELLOW);
        ledEnable(LED_GREEN);
        SysCtlDelayMS(500);
    }
}