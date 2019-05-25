#include <led.h>

#include <driverlib/sysctl.h>

void ledInit() {
    /* enable GPIO */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)));

    /* default LED outputs */
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, LED_RED | LED_YELLOW | LED_GREEN);
}
