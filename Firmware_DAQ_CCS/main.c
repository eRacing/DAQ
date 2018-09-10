//*****************************************************************************
// 
// Titre : Soumission d'information d'un bus CAN à un périphérique UART
//
// Fichier : telemetrie.c (et autres librairies TivaWare)
//
// Auteur : Simon Bellemare, Polytechnique Montréal
//					et librairies TI TivaWare (lorsque mentionné).
//
// Date : 15 avril 2014
//
// Contexte : Projet personnel en génie électrique, ELE3000
// 
// Les librairies incluses dans ce projet proviennet de la revision 2.1.0.12573 
// du DK-TM4C123G Firmware Package de TI TivaWare.
//
//*****************************************************************************


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "grlib/grlib.h"
#include "utils/uartstdio.h"
#include "driverlib/interrupt.h"

//*****************************************************************************
//
// A counter that keeps track of the number of times the RX interrupt has
// occurred.
//
//*****************************************************************************
volatile uint32_t g_ui32RXMsgCount = 0;

//*****************************************************************************
//
// A flag for the interrupt handler to indicate that a message was received.
//
//*****************************************************************************
volatile bool g_bRXFlag = 0;

//*****************************************************************************
//
// A global to keep track of the error flags that have been thrown so they may
// be processed.
//
//*****************************************************************************
volatile uint32_t g_ui32ErrFlag = 0;

//*****************************************************************************
//
// CAN message Objects for data being received
//
//*****************************************************************************
tCANMsgObject g_sCAN0RxMessage;

//*****************************************************************************
//
// Message Identifiers and Objects
// RXID is set to 0 so all messages are received
//
//*****************************************************************************
#define CAN0RXID                0
#define RXOBJECT                1

//*****************************************************************************
//
// Variables to hold data being reveived
//
//*****************************************************************************
uint32_t g_ui32RXMsgData;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// CAN 0 Interrupt Handler. It checks for the cause of the interrupt, and
// maintains a count of all messages that have been received
//
// *This function comes directly from the TI TivaWare package.
//
//*****************************************************************************
void
CAN0IntHandler(void)
{
    uint32_t ui32Status;

    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
    // CAN_INT_STS_CAUSE register values
    // 0x0000        = No Interrupt Pending
    // 0x0001-0x0020 = Number of message object that caused the interrupt
    // 0x8000        = Status interrupt
    // all other numbers are reserved and have no meaning in this system
    //
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    //
    // If this was a status interrupt acknowledge it by reading the CAN
    // controller status register.
    //
    if(ui32Status == CAN_INT_INTID_STATUS)
    {
        //
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors. Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.
        //
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

        //
        // Add ERROR flags to list of current errors. To be handled
        // later, because it would take too much time here in the
        // interrupt.
        //
        g_ui32ErrFlag |= ui32Status;
    }

    //
    // Check if the cause is message object RXOBJECT, which we are using
    // for receiving messages.
    //
    else if(ui32Status == RXOBJECT)
    {
        //
        // Getting to this point means that the RX interrupt occurred on
        // message object RXOBJECT, and the message reception is complete.
        // Clear the message object interrupt.
        //
        CANIntClear(CAN0_BASE, RXOBJECT);

        //
        // Increment a counter to keep track of how many messages have been
        // received.  In a real application this could be used to set flags to
        // indicate when a message is received.
        //
        g_ui32RXMsgCount++;

        //
        // Set flag to indicate received message is pending.
        //
        g_bRXFlag = true;

        //
        // Since a message was received, clear any error flags.
        // This is done because before the message is received it triggers
        // a Status Interrupt for RX complete. by clearing the flag here we
        // prevent unnecessary error handling from happeneing
        //
        g_ui32ErrFlag = 0;
    }

    //
    // Otherwise, something unexpected caused the interrupt.  This should
    // never happen.
    //
    else
    {
        //
        // Spurious interrupt handling can go here.
        //
    }
}

//*****************************************************************************
//
// Configure the UART and its pins.
//
// *This function comes directly from the TI TivaWare package.
//
//*****************************************************************************
void
ConfigureUART(void)
{
    
	  //
    // Enable the GPIO Peripheral used by the UART.
    //
		MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Enable UART1
    //
		MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

    //
    // Configure GPIO Pins for UART mode.
    //
	MAP_GPIOPinConfigure(GPIO_PB0_U1RX);
    MAP_GPIOPinConfigure(GPIO_PB1_U1TX);
    MAP_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
		UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
		UARTStdioConfig(1, 115200, 16000000);
}

//*****************************************************************************
//
// Setup CAN0 to both send and receive at 500KHz.
//
//*****************************************************************************
void
InitCAN0(void)
{

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);

    // Enable the alternate function on the GPIO pins.
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    // Initialize the CAN controller
    CANInit(CAN0_BASE);

    // Set up the bit rate for the CAN bus.  
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);

    // Enable interrupts on the CAN peripheral.
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    // Enable the CAN interrupt on the processor (NVIC).
    IntEnable(INT_CAN0);

    // Enable the CAN for operation.
    CANEnable(CAN0_BASE);

    // Initialize a message object to be used for receiving CAN messages with
    // any CAN ID. 
    g_sCAN0RxMessage.ui32MsgID = CAN0RXID;
    g_sCAN0RxMessage.ui32MsgIDMask = 0;
    g_sCAN0RxMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    g_sCAN0RxMessage.ui32MsgLen = sizeof(g_ui32RXMsgData);

    // Now load the message object into the CAN peripheral.
    CANMessageSet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, MSG_OBJ_TYPE_RX);
}

//*****************************************************************************
//
// Can ERROR handling. When a message is received if there is an erro it is
// saved to g_ui32ErrFlag, the Error Flag Set. Below the flags are checked
// and cleared. It is left up to the user to add handling fuctionality if so
// desiered.
//
// *This function comes directly from the TI TivaWare package.
//
//*****************************************************************************
void
CANErrorHandler(void)
{
    //
    // CAN controller has entered a Bus Off state.
    //
    if(g_ui32ErrFlag & CAN_STATUS_BUS_OFF)
    {
        //
        // Handle Error Condition here
        //
        UARTprintf("    ERROR: CAN_STATUS_BUS_OFF \n");

        //
        // Clear CAN_STATUS_BUS_OFF Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_BUS_OFF);

    }

    //
    // CAN controller error level has reached warning level.
    //
    if(g_ui32ErrFlag & CAN_STATUS_EWARN)
    {
        //
        // Handle Error Condition here
        //
        //UARTprintf("    ERROR: CAN_STATUS_EWARN \n");

        //
        // Clear CAN_STATUS_EWARN Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_EWARN);
    }

    //
    // CAN controller error level has reached error passive level.
    //
    if(g_ui32ErrFlag & CAN_STATUS_EPASS)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_EPASS Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_EPASS);
    }

    //
    // A message was received successfully since the last read of this status.
    //
    if(g_ui32ErrFlag & CAN_STATUS_RXOK)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_RXOK Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_RXOK);
    }

    //
    // A message was transmitted successfully since the last read of this
    // status.
    //
    if(g_ui32ErrFlag & CAN_STATUS_TXOK)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_TXOK Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_TXOK);
    }

    //
    // This is the mask for the last error code field.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_MSK)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_LEC_MSK Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_MSK);
    }

    //
    // A bit stuffing error has occurred.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_STUFF)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_LEC_STUFF Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_STUFF);
    }

    //
    // A formatting error has occurred.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_FORM)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_LEC_FORM Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_FORM);
    }

    //
    // An acknowledge error has occurred.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_ACK)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_LEC_ACK Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_ACK);
    }

    //
    // The bus remained a bit level of 1 for longer than is allowed.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_BIT1)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_LEC_BIT1 Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_BIT1);
    }

    //
    // The bus remained a bit level of 0 for longer than is allowed.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_BIT0)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_LEC_BIT0 Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_BIT0);
    }

    //
    // A CRC error has occurred.
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_CRC)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_LEC_CRC Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_CRC);
    }

    //
    // This is the mask for the CAN Last Error Code (LEC).
    //
    if(g_ui32ErrFlag & CAN_STATUS_LEC_MASK)
    {
        //
        // Handle Error Condition here
        //

        //
        // Clear CAN_STATUS_LEC_MASK Flag
        //
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_MASK);
    }

    //
    // If there are any bits still set in g_ui32ErrFlag then something unhandled
    // has happened. Print the value of g_ui32ErrFlag.
    //
    if(g_ui32ErrFlag !=0)
    {
        UARTprintf("    Unhandled ERROR: %x \n",g_ui32ErrFlag);
    }
}

//*****************************************************************************
//
// Fonction principale pour les initialisations et le traitement des 
// interruptins CAN et l'envoi sur l'UART.
//
//*****************************************************************************
int main(void)
{
    // Set the clocking to run directly from the crystal.
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
	  
    // Initialize the UART
    ConfigureUART();

    // Initialize CAN0
    InitCAN0(); 

    while(1)
    {
			  //
        // If the flag is set, that means that the RX interrupt occurred and
        // there is a message ready to be read from the CAN
        //
        if(g_bRXFlag)
        {
						//
            // Reuse the same message object that was used earlier to configure
            // the CAN for receiving messages.
					  //
            g_sCAN0RxMessage.pui8MsgData = (uint8_t *) &g_ui32RXMsgData;

            //
            // Read the message from the CAN.  Message object RXOBJECT is used.
            //
            CANMessageGet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, 0);

            //
            // Clear the pending message flag.
            //
            g_bRXFlag = 0;

            //
            // Check to see if there is an indication that some messages were
            // lost.
            //
            if(g_sCAN0RxMessage.ui32Flags & MSG_OBJ_DATA_LOST)
            {
                UARTprintf("\nCAN message loss detected\n");
            }

            //
            // Print the received character to the UART
						// The format is <ID>:<Data>
            //
						UARTprintf("%d", g_sCAN0RxMessage.ui32MsgID); // print ID
						UARTprintf("%c", ':'); // print ID
						UARTprintf("%d", g_ui32RXMsgData); //Print MSG
        }
        else
        {
					  //
            // CAN Error Handling
            //
            if(g_ui32ErrFlag != 0)
            {
                CANErrorHandler();
            }
        } 
    }
}
