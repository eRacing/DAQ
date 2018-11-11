/*
 * timer_lib.c
 *
 *  Created on: May 29, 2018
 *      Author: Felix Collin
 */

#include "timer_lib.h"
#include "diskio.h"

void disk_timerproc (void);
DWORD get_fattime (void);

volatile uint32_t tickMs = 0;

void Timer0IntHandler(void){
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    tickMs++;

    if((tickMs % 10) == 0){
        disk_timerproc();
    }
}

void InitTimer(void){
    if(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)){}
    }

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    uint32_t ui32Period = (SysCtlClockGet() / 1000) / 2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);

    TimerIntRegister(TIMER0_BASE,TIMER_A,Timer0IntHandler);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    TimerEnable(TIMER0_BASE, TIMER_A);
}

uint32_t GetTickMs(void){
    return tickMs;
}

void DelayMs(uint32_t ms){
    uint32_t tick = GetTickMs();
    while((GetTickMs()-tick)<ms);
}

