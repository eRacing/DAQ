#include <stdbool.h>
#include <stdint.h>
#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>
#include <sdcard.h>

#define SysCtlDelayMS(MS) SysCtlDelay((int) ((((double) SysCtlClockGet()) * ((double) (MS))) / 3000.0f))

////////////////
// Debug LEDs //
////////////////

#define DEBUG_LED_RED    GPIO_PIN_1
#define DEBUG_LED_YELLOW GPIO_PIN_2
#define DEBUG_LED_GREEN  GPIO_PIN_3

void enableDebugLED(int led) {
    GPIOPinWrite(GPIO_PORTD_BASE, led, 0);
}

void disableDebugLED(int led) {
    GPIOPinWrite(GPIO_PORTD_BASE, led, led);
}

////////////////
// Public API //
////////////////

void daqInit() {
    /* setup 40MHz clock */
    SysCtlClockSet(SYSCTL_SYSDIV_5 |SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    /* enable GPIO */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)));
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, DEBUG_LED_RED | DEBUG_LED_YELLOW | DEBUG_LED_GREEN);

    /* enable SSI */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_SSI2)));

    sdcardInit();
}

void daqLoop() {
    while(1) {
        /* TODO: do something useful with the DAQ */
        enableDebugLED(DEBUG_LED_RED);
        disableDebugLED(DEBUG_LED_YELLOW);
        disableDebugLED(DEBUG_LED_GREEN);
        SysCtlDelayMS(500);

        disableDebugLED(DEBUG_LED_RED);
        enableDebugLED(DEBUG_LED_YELLOW);
        disableDebugLED(DEBUG_LED_GREEN);
        SysCtlDelayMS(500);

        disableDebugLED(DEBUG_LED_RED);
        disableDebugLED(DEBUG_LED_YELLOW);
        enableDebugLED(DEBUG_LED_GREEN);
        SysCtlDelayMS(500);
    }
}
