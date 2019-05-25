#ifndef INCLUDE_LED_H_
#define INCLUDE_LED_H_

#include <stdbool.h>
#include <stdint.h>
#include <driverlib/gpio.h>
#include <inc/hw_memmap.h>

void ledInit();

enum led {
    LED_RED    = GPIO_PIN_1,
    LED_YELLOW = GPIO_PIN_2,
    LED_GREEN  = GPIO_PIN_3,
};

static inline void ledEnable(enum led led) {
    GPIOPinWrite(GPIO_PORTD_BASE, led, 0);
}

static inline void ledDisable(enum led led) {
    GPIOPinWrite(GPIO_PORTD_BASE, led, led);
}

static inline void ledEnableRed() {
    ledEnable(LED_RED);
}

static inline void ledEnableYellow() {
    ledEnable(LED_YELLOW);
}

static inline void ledEnableGreen() {
    ledEnable(LED_GREEN);
}

static inline void ledDisableRed() {
    ledDisable(LED_RED);
}

static inline void ledDisableYellow() {
    ledDisable(LED_YELLOW);
}

static inline void ledDisableGreen() {
    ledDisable(LED_GREEN);
}

#endif
