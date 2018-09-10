/*
 * can_lib.c
 *
 *  Created on: Jun 18, 2018
 *      Author: Felix Collin
 */

#include "can_lib.h"
#include "io_lib.h"
#include "timer_lib.h"
#include "buffer.h"

volatile struct circular canmessages = CIRCULAR_INIT;

msgEntry sCANMessagesBuffer[CAN_BUFFER_SIZE];
volatile uint32_t canBufferPosition = 0;

//*****************************************************************************
//
// A counter that keeps track of the number of times the RX interrupt has
// occurred, which should match the number of messages that were received.
//
//*****************************************************************************
volatile uint32_t g_ui32MsgCount = 0;

//*****************************************************************************
//
// A flag to indicate that some reception error occurred.
//
//*****************************************************************************
volatile bool g_bErrFlag = 0;

extern volatile uint32_t ticksCan = 0;

//*****************************************************************************
//
// This function is the interrupt handler for the CAN peripheral.  It checks
// for the cause of the interrupt, and maintains a count of all messages that
// have been received.
//
//*****************************************************************************
void
CANIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    //
    // If the cause is a controller status interrupt, then get the status
    //
    if(ui32Status == CAN_INT_INTID_STATUS)
    {
        //
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors.  Error processing
        // is not done in this example for simplicity.  Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.
        //
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

        //
        // Set a flag to indicate some errors may have occurred.
        //
        g_bErrFlag = 1;
    }

    //
    // Check if the cause is message object RX_MSG_OBJ.
    //
    else if(ui32Status == RX_MSG_OBJ)
    {
        tCANMsgObject CANRxMessage;
        uint8_t CANRxMsgData[8];

        //
        // Getting to this point means that the RX interrupt occurred on
        // message object RX_MSG_OBJ, and the message reception is complete.  Clear the
        // message object interrupt.
        //
        CANIntClear(CAN0_BASE, RX_MSG_OBJ);

        //
        // Increment a counter to keep track of how many messages have been
        // received.  In a real application this could be used to set flags to
        // indicate when a message is received.
        //
        g_ui32MsgCount++;

        //
        // Since a message was received, clear any error flags.
        //
        g_bErrFlag = 0;

        ToggleLed();

        CANRxMessage.pui8MsgData = CANRxMsgData;

        // Read the message from the CAN.  Message object RX_MSG_OBJ is used
        CANMessageGet(CAN0_BASE, RX_MSG_OBJ, &CANRxMessage, 0);

        /* get information of the message */
        uint32_t timestamp = GetTickMs();
        uint32_t number = g_ui32MsgCount;
        uint16_t id = CANRxMessage.ui32MsgID;
        uint16_t len = CANRxMessage.ui32MsgLen;

        /* serialize the message */
        struct buffer_entry entry = {
            .data = {
                 (timestamp & 0x000000FF) >>  0, /* 0 */
                 (timestamp & 0x0000FF00) >>  8, /* 1 */
                 (timestamp & 0x00FF0000) >> 16, /* 2 */
                 (timestamp & 0xFF000000) >> 24, /* 3 */
                 (number & 0x000000FF) >>  0,    /* 4 */
                 (number & 0x0000FF00) >>  8,    /* 5 */
                 (number & 0x00FF0000) >> 16,    /* 6 */
                 (number & 0xFF000000) >> 24,    /* 7 */
                 (id & 0x00FF) >>  0,            /* 8 */
                 (id & 0xFF00) >>  8,            /* 9 */
            },
        };

        /* copy the data bytes */
        for(int i = 0; i < 8; i++) {
            entry.data[i+10] = (i < len) ? CANRxMessage.pui8MsgData[i] : 0x00;
        }

        /* put the data into the circular buffer */
         circular_insert((struct circular*) &canmessages, &entry);

        /* update CAN last message ticks */
        ticksCan = GetTickMs();
    }
    else
    {
        //
        // Spurious interrupt handling can go here.
        //
    }
}

void InitCan(void){
    tCANMsgObject sCANMessage;
    //
    // CAN0 is used with RX and TX pins on port E4 and E5.
    // GPIO port E needs to be enabled so these pins can be used.
    //
    if(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)){
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)){}
    }

    //
    // Configure the GPIO pin muxing to select CAN0 functions for these pins.
    // This step selects which alternate function is available for these pins.
    //
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);

    //
    // Enable the alternate function on the GPIO pins.  The above step selects
    // which alternate function is available.  This step actually enables the
    // alternate function instead of GPIO for these pins.
    //
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    //
    // The GPIO port and pins have been set up for CAN.  The CAN peripheral
    // must be enabled.
    //
    if(!SysCtlPeripheralReady(SYSCTL_PERIPH_CAN0)){
        SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_CAN0)){}
    }

    //
    // Initialize the CAN controller
    //
    CANInit(CAN0_BASE);

    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), CAN_BITRATE);

    //
    // Enable interrupts on the CAN peripheral.  This example uses static
    // allocation of interrupt handlers which means the name of the handler
    // is in the vector table of startup code.  If you want to use dynamic
    // allocation of the vector table, then you must also call CANIntRegister()
    // here.
    CANIntRegister(CAN0_BASE, CANIntHandler); // if using dynamic vectors
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    //
    // Enable the CAN interrupt on the processor (NVIC).
    //
    IntEnable(INT_CAN0);

    //
    // Enable the CAN for operation.
    //
    CANEnable(CAN0_BASE);

    //
    // Initialize a message object to receive CAN messages with ID 0x1001.
    // The expected ID must be set along with the mask to indicate that all
    // bits in the ID must match.
    //
    sCANMessage.ui32MsgID = 0x0;
    sCANMessage.ui32MsgIDMask = 0x0;
    sCANMessage.ui32Flags = (MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER);
    sCANMessage.ui32MsgLen = 8;

    //
    // Now load the message object into the CAN peripheral message object 1.
    // Once loaded the CAN will receive any messages with this CAN ID into
    // this message object, and an interrupt will occur.
    //
    CANMessageSet(CAN0_BASE, RX_MSG_OBJ, &sCANMessage, MSG_OBJ_TYPE_RX);

}



