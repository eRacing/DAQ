#include "can_mng.h"
#include "commands.h"
#include "spi_lib.h"

// TM4C specific
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"

#define BUFFER_SIZE 22
#define SPI_CHANNEL 0

#define XBEE_RESET GPIO_PIN_7 // Port A
#define XBEE_ATN GPIO_PIN_6 // Port A
#define XBEE_DOUT GPIO_PIN_0 // Port B

static uint32_t ip_address = 0xFFFFFFFF; // IPv4 address for packet transmission
static uint16_t dest_port = 0x2616;	// UDP port for packet tansmission
static uint16_t src_port = 0x2616;	// UDP port for packet tansmission

//Function header
void SPI_send_buffer(uint8_t channel, uint8_t* buffer, uint8_t buffer_size);
void xbee_send_UDP_packet(uint8_t *payload, uint8_t size);

void delay(uint32_t milliseconds)
{
    MAP_SysCtlDelay((MAP_SysCtlClockGet() / 3) * (milliseconds / 1000.0f));
}

void xbee_init(void)
{
    // src_port = get configuration from EEPROM
    // dest_port = get configuration from EEPROM

    // Enable ports
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while (!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
    {
    }

    //MAP_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, XBEE_ATTN);

    /*
     // SPI MODE ENABLE

     MAP_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, XBEE_RESET);

     // Set DOUT low (to start in SPI mode)
     MAP_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, XBEE_DOUT);
     MAP_GPIOPinWrite(GPIO_PORTB_BASE, XBEE_DOUT, 0x0);

     // Reset xbee
     MAP_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, XBEE_RESET);
     MAP_GPIOPinWrite(GPIO_PORTA_BASE, XBEE_RESET, 0x0);

     // Wait
     delay(100);

     // Desactivate reset
     MAP_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, XBEE_RESET);
     MAP_GPIOPinWrite(GPIO_PORTA_BASE, XBEE_RESET, XBEE_RESET);

     MAP_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, XBEE_DOUT);

     // Wait for ATTN
     //while (MAP_GPIOPinRead(GPIO_PORTA_BASE, RESET_ATTN) == ){}

     */

    // SPI CONFIGURATION
    SPI_Config_t spi_config;
    spi_config.cpol_ = 0;
    spi_config.cpha_ = 0;
    spi_config.mode_ = 0;
    spi_config.bitRate_ = 9600;
    spi_config.dataWidth_ = 8;

    SPI_init(SPI_CHANNEL, spi_config);

    uint8_t start[3] = { '+', '+', '+' };
    uint8_t end[1] = { 0x0D };

    uint8_t cmd1[9] = { 0x7E, 0x00, 0x05, 0x09, 0x07, 0x49, 0x50, 0x01, 0x55 };
    uint8_t cmd2[8] = { 0x7E, 0x00, 0x04, 0x09, 0x08, 0x57, 0x52, 0x45 };
    uint8_t cmd3[8] = { 0x7E, 0x00, 0x04, 0x09, 0x09, 0x41, 0x43, 0x69 };
    uint8_t cmd4[9] = { 0x7E, 0x00, 0x05, 0x09, 0x0A, 0x49, 0x50, 0x00, 0x53 };
    uint8_t cmd5[8] = { 0x7E, 0x00, 0x04, 0x09, 0x0B, 0x57, 0x52, 0x42 };
    uint8_t cmd6[8] = { 0x7E, 0x00, 0x04, 0x09, 0x0C, 0x41, 0x43, 0x66 };

    delay(2000);

    SPI_send_buffer(SPI_CHANNEL, start, 3);
    delay(1000);
    SPI_send_buffer(SPI_CHANNEL, cmd1, 9);
    SPI_send_buffer(SPI_CHANNEL, end, 1);
    delay(100);
    SPI_send_buffer(SPI_CHANNEL, cmd2, 8);
    SPI_send_buffer(SPI_CHANNEL, end, 1);
    delay(100);
    SPI_send_buffer(SPI_CHANNEL, cmd3, 8);
    SPI_send_buffer(SPI_CHANNEL, end, 1);
    delay(100);
    SPI_send_buffer(SPI_CHANNEL, cmd4, 9);
    SPI_send_buffer(SPI_CHANNEL, end, 1);
    delay(100);
    SPI_send_buffer(SPI_CHANNEL, cmd5, 8);
    SPI_send_buffer(SPI_CHANNEL, end, 1);
    delay(100);
    SPI_send_buffer(SPI_CHANNEL, cmd6, 8);
    SPI_send_buffer(SPI_CHANNEL, end, 1);

    delay(2000);

    delay(1000);

}

void xbee_send_CAN_message(CAN_log_message_t msg)
{
    uint8_t i;
    uint8_t buf[BUFFER_SIZE];

    //	CAN Rx message
    //	Direction : DAQ module -> Base station (PC)
    //	Packet format (in order) :
    //		- CMD_CAN_RX (1 byte)
    //		- message number (4 bytes, uint32, big endian)
    //		- timestamp (4 bytes, uint32, tenth of ms, big endian)
    //		- CAN msg ID (4 bytes, uint32)
    //		- DLC (1 byte, uint8)
    //		- Data (8 bytes, padding of 0 if DLC < 8)

    // Command number
    buf[0] = CMD_CAN_RX;

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

    xbee_send_UDP_packet(buf, BUFFER_SIZE);

}

void xbee_send_UDP_packet(uint8_t *payload, uint8_t size)
{

    uint8_t i;
    uint8_t c = 0;
    uint16_t length = size + 12;
    uint8_t buf[100]; //length+2 pas de dynamique
    uint8_t checksum;

    //		- Start delimiter (1 byte)
    buf[0] = (uint8_t) (0x7E);

    //      - Length (1 byte)
    buf[2] = (uint8_t) length;
    buf[1] = (uint8_t) (length >> 8);

    //      - API frame identifier (0x20)
    buf[3] = (uint8_t) (0x20);

    //      - Frame ID
    buf[4] = (uint8_t) (0x01);

    //      - IPv4 32-bit destination address
    buf[8] = (uint8_t) ip_address;
    buf[7] = (uint8_t) (ip_address >> 8);
    buf[6] = (uint8_t) (ip_address >> 16);
    buf[5] = (uint8_t) (ip_address >> 24);

    //      - 16-bit destination port
    buf[10] = (uint8_t) dest_port;
    buf[9] = (uint8_t) (dest_port >> 8);

    //      - 16-bit source port
    buf[12] = (uint8_t) src_port;
    buf[11] = (uint8_t) (src_port >> 8);

    //      - Protocol (UDP=0)
    buf[13] = 0;

    //      - Transmit options bitfield (Ignore for UDP:=0)
    buf[14] = 0;

    //      - Data (max 1400 bytes)
    for (i = 0; i < size; i++)
    {
        buf[15 + i] = payload[i];
    }

    //      - Checksum
    for (i = 3; i < 3 + length; i++)
    {
        c = c + buf[i];
    }
    checksum = 0xFF - c;
    buf[3 + length] = checksum;

    uint8_t message[22] = { 0x7E, 0x00, 0x7D, 0x31, 0x20, 0x23, 0xFF, 0xFF,
                            0xFF, 0xFF, 0x26, 0x16, 0x26, 0x16, 0x00, 0x00,
                            0x48, 0x45, 0x4C, 0x4C, 0x4F, 0xD4 };  //HELLO
    uint8_t message2[24] = { 0x61, 0x62, 0x63, 0x61, 0x62, 0x63, 0x61, 0x62,
                             0x63, 0x61, 0x62, 0x63, 0x61, 0x62, 0x63, 0x61,
                             0x62, 0x63, 0x61, 0x62, 0x63, 0x61, 0x62, 0x63 };
    uint8_t message3[17] = { 0x7E, 0x00, 0x0D, 0x20, 0x01, 0xFF, 0xFF, 0xFF,
                             0xFF, 0x26, 0x16, 0x26, 0x16, 0x00, 0x00, 0x41,
                             0x29 };
    uint8_t message4[21] = { 0x7E, 0x00, 0x11, 0x20, 0x01, 0xFF, 0xFF, 0xFF,
                             0xFF, 0x26, 0x16, 0x26, 0x16, 0x00, 0x00, 0x48,
                             0x45, 0x4C, 0x4C, 0x4F, 0xF6 };

    //TODO: fix size/length
    //TODO spi chip select?
    //MAP_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3); //chip select
    SPI_send_buffer(SPI_CHANNEL, buf, length + 4);
    //  SPI_send_buffer(SPI_CHANNEL, message3, 17);
    //MAP_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x0); //chip select

}

void SPI_send_buffer(uint8_t channel, uint8_t* buffer, uint8_t buffer_size)
{
    int i;
    for (i = 0; i < buffer_size; i++)
    {
        SPI_send8Bits(channel, buffer[i]);
    }
}

