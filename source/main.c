#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/eeprom.h"

#include "fatfs/src/ff.h"
#include "fatfs/src/diskio.h"

#include "system_lib.h"
#include "timer_lib.h"
#include "can_lib.h"
#include "can_mng.h"
#include "xbee.h"

#ifndef NULL
#define NULL          0
#endif

#define CAN_BUS 0

#define BUFFER_SIZE 22
#define MULTIPLIER 512

static uint8_t write_buffer[BUFFER_SIZE*MULTIPLIER];

static uint32_t time_ms = 0;

static FATFS fs;
static FIL file;

static char file_name[20];

void convert(CAN_log_message_t msg, uint8_t *buf);

//*****************************************************************************
//
// This is the handler for this SysTick interrupt.  FatFs requires a timer tick
// every 10 ms for internal timing purposes.
//
//*****************************************************************************
void SysTickHandler(void)
{
    // Call the FatFs tick timer.
    disk_timerproc();

    time_ms++;
}

void init_sd(void)
{
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    MAP_FPULazyStackingEnable();

    // Set the system clock to run at 50MHz from the PLL.
    //MAP_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Enable the peripherals used by this example.
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)))
        ;
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_SSI2)))
        ;

    // Configure SysTick for a 100Hz interrupt. The FatFs driver wants a 10 ms tick.
    MAP_SysTickDisable();
    MAP_SysTickPeriodSet(MAP_SysCtlClockGet() / 100);
    SysTickIntRegister(SysTickHandler);
    MAP_SysTickIntEnable();
    MAP_SysTickEnable();

    // Enable Interrupts
    MAP_IntMasterEnable();
}

void init_led(void) {
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)))
        ;

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x01;

    MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
}

void init_eeprom() {

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    while (!(MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0)))
        ;

    EEPROMInit();

}

uint32_t eeprom_next_number() {

//    uint32_t data = 1;
//    MAP_EEPROMProgram(&data, 0x00, 4);

    uint32_t read_data;
    MAP_EEPROMRead(&read_data, 0x00, 4);

    read_data++;
    MAP_EEPROMProgram(&read_data, 0x00, 4);

    return read_data;
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

int main(void)
{

    // Initialize Clock and Timer
    Clock_speed_t sys_config = { MAXIMUM };
    sys_config_clock(sys_config);

    //timer_init();

    init_sd();
    init_led();

    // Initialize CAN
    CAN_Config_t can_config;
    can_config.bit_rate = BITRATE_500000;
    can_config.mask = 0;
    can_config.buffer_behaviour = OVERWRITE_OLDEST_MESSAGE;
    can_config.pin_selection = 0;
    CAN_init(CAN_BUS, can_config);

    // Initialize EEPROM
    // TODO
    init_eeprom();
    uint32_t file_number;
    file_number = eeprom_next_number();


    // Initialize XBee module
    //delay_ms(5000);
    //xbee_init();


    delay_ms(1000);

    FRESULT result;

    file_name[0] = '/';
    file_name[1] = 'l';
    file_name[2] = 'o';
    file_name[3] = 'g';

    itoa(file_number, file_name+4);

    result = f_mount(0, &fs);
    result = f_open(&file, file_name, (FA_WRITE | FA_CREATE_ALWAYS));

    CAN_message_t rx_msg;
    CAN_log_message_t log_msg;
    uint32_t msg_number = 0;
    int i;

//    CAN_message_t tx_msg = { 42, 8, {'a','b','c','d','e','f','g','h'} };
//    CAN_log_message_t log_msg = { rx_msg, 1527184191, msg_number };

    int buf_index = 0;

    while (1)
    {

        while (CAN_get_buffer_size(CAN_BUS) > 0) {
            // Read CAN message from bus
            CAN_read_message(CAN_BUS, &rx_msg);

            // Create CAN log message
            log_msg.msg.id = rx_msg.id;
            log_msg.msg.length = rx_msg.length;
            for (i=0; i<rx_msg.length; i++) {
                log_msg.msg.data[i] = rx_msg.data[i];
            }
            log_msg.timestamp = time_ms;//getTime_ms();
            log_msg.number = msg_number;

            // Send CAN log message to XBee
            //xbee_send_CAN_message(log_msg);

            // Write CAN log message to SD Card

            uint8_t buf[BUFFER_SIZE];
            convert(log_msg, buf);
            for (i=0; i<BUFFER_SIZE; i++) {
                write_buffer[buf_index + i] = buf[i];
            }
            buf_index = buf_index + BUFFER_SIZE;

            if (buf_index >= BUFFER_SIZE*MULTIPLIER) {
                MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
                uint32_t bytes_written;
                result = f_write(&file, write_buffer, BUFFER_SIZE*MULTIPLIER, &bytes_written);
                result = f_sync(&file);
                MAP_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);
                buf_index = 0;
            }

            msg_number++;

//            int y;
//            for (y=0; y<1000; y++) {
//
//            }
        }



        // Test message
//        CAN_message_t rx_msg_test = { 42, 4, {'a','b','c','d'} };
//        CAN_log_message_t logmsg_test = { rx_msg_test, 341245, msg_number };
//        xbee_send_CAN_message(logmsg_test);
//        msg_number++;
//        delay_ms(100);
    }

}

void convert(CAN_log_message_t msg, uint8_t *buf)
{
    uint8_t i;
    //uint8_t buf[BUFFER_SIZE];

    //  CAN Rx message
    //  Direction : DAQ module -> Base station (PC)
    //  Packet format (in order) :
    //      - CMD_CAN_RX (1 byte)
    //      - message number (4 bytes, uint32, big endian)
    //      - timestamp (4 bytes, uint32, ms, big endian)
    //      - CAN msg ID (4 bytes, uint32)
    //      - DLC (1 byte, uint8)
    //      - Data (8 bytes, padding of 0 if DLC < 8)

    // Command number
    buf[0] = 0x0D; //CMD_CAN_RX;

    // Message number
    buf[4] = (uint8_t) msg.number;
    buf[3] = (uint8_t) (msg.number >> 8);
    buf[2] = (uint8_t) (msg.number >> 16);
    buf[1] = (uint8_t) (msg.number >> 24);

    // Timestamp
    buf[8] = (uint8_t) msg.timestamp;
    buf[7] = (uint8_t) (msg.timestamp >> 8);
    buf[6] = (uint8_t) (msg.timestamp >> 16);
    buf[5] = (uint8_t) (msg.timestamp >> 24);

    // CAN message ID
    buf[12] = (uint8_t) msg.msg.id;
    buf[11] = (uint8_t) (msg.msg.id >> 8);
    buf[10] = (uint8_t) (msg.msg.id >> 16);
    buf[9] = (uint8_t) (msg.msg.id >> 24);

    // DLC
    buf[13] = msg.msg.length;

    // Data
    for (i = 0; i < msg.msg.length; i++)
    {
        buf[14 + i] = msg.msg.data[i];
    }

    // Padding with 0
    for (i = 14 + msg.msg.length; i < BUFFER_SIZE; i++)
    {
        buf[i] = 0;
    }

}

