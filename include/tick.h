#ifndef INCLUDE_TICK_H_
#define INCLUDE_TICK_H_

#include <stdint.h>

extern volatile uint32_t systemTick;

void tickInit(void);

#define getSystemTick() systemTick

#endif
