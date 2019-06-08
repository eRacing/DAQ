
#include <utils.h>
#include <config.h>
#include <stdint.h>
#include <stdbool.h>
#include <can.h>

#include <driverlib/gpio.h>
#include <driverlib/can.h>
#include <driverlib/interrupt.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>

#define RINGBUFFER_DECLARE_DATA_TYPE
#include <ringbuffer.h>

//////////
// Data //
//////////

#define RX_OBJ_ID 1

RINGBUFFER_DECLARE(
    can,
    struct messsage,
    CAN_RINGBUFFER_SUBBUF_SIZE,
    CAN_RINGBUFFER_SUBBUF_COUNT
)

static struct can_rb ringbuffer;

///////////////
// Interrupt //
///////////////

void canInterrupt() {
    uint32_t status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);
    CANIntClear(CAN0_BASE, status);

    if(status == CAN_INT_INTID_STATUS) {
        status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        return;
    }

    if(status != RX_OBJ_ID) {
        return;
    }

    struct messsage msg = CAN_MESSAGE_INIT;
    tCANMsgObject canObject;
    canObject.pui8MsgData = msg.data;
    CANMessageGet(CAN0_BASE, RX_OBJ_ID, &canObject, 0);

    /* TODO: get timestamp */

    msg.id = canObject.ui32MsgID;
    can_rb_insert(&ringbuffer, &msg);
}

////////////////
// Public API //
////////////////

void canInit() {
    ringbuffer = can_rb_init;

    /* initialize the GPIO peripheral for RX/TX pins */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)));

    /* select CAN functionalities for the GPIO pins */
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    /* initialize the CAN peripheral */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_CAN0)));

    /* setup the CAN peripheral */
    CANInit(CAN0_BASE);
    assert(CANBitRateSet(CAN0_BASE, SysCtlClockGet(), CAN_BITRATE) == CAN_BITRATE);

    /* configure CAN interrupt */
    CANIntRegister(CAN0_BASE, &canInterrupt);
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    /* enable interrupt */
    IntEnable(INT_CAN0);

    /* enable the CAN peripheral */
    CANEnable(CAN0_BASE);

    /* configure a box to receive all messages */
    tCANMsgObject rxBoxConfig = {
        .ui32MsgID = 0x0,
        .ui32MsgIDMask = 0x0,
        .ui32Flags = (MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER),
        .ui32MsgLen = 8,
    };
    CANMessageSet(CAN0_BASE, RX_OBJ_ID, &rxBoxConfig, MSG_OBJ_TYPE_RX);
}

void* canGetSubbufferBlocking(size_t* size) {
    struct can_rb_subbuf* subbuf = can_rb_get_subbuf(&ringbuffer);
    size[0] = sizeof(subbuf->entries);
    return (void*) subbuf;
}

void canFlushSubbuffer(void* subbuf) {
    can_rb_subbuf_flush((struct can_rb_subbuf*) subbuf);
}
