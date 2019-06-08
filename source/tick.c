#include <stdbool.h>
#include <tick.h>

#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <inc/hw_memmap.h>

//////////
// Data //
//////////

volatile uint32_t systemTick = 0;

///////////////
// Interrupt //
///////////////

static void tickInterrupt() {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    systemTick++;
}

////////////////
// Public API //
////////////////

void tickInit() {
    /* initialize the timer peripheral for the system tick */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)));

    /* configure the timer to have a period of 1 ms */
    uint32_t period = SysCtlClockGet() / 1000;
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, period);

    /* enable interrupt for this timer */
    TimerIntRegister(TIMER0_BASE, TIMER_A, tickInterrupt);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    /* enable the timer peripheral */
    TimerEnable(TIMER0_BASE, TIMER_A);
}
