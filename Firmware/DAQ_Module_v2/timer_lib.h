/*
 * timer_lib.h
 *
 *  Created on: May 29, 2018
 *      Author: Felix Collin
 */

#ifndef TIMER_LIB_H_
#define TIMER_LIB_H_

#include "globals_lib.h"
#include "driverlib/timer.h"

extern volatile uint32_t tickMs;

void InitTimer(void);
uint32_t GetTickMs(void);
void DelayMs(uint32_t ms);

#endif /* TIMER_LIB_H_ */
