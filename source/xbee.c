#include "can_mng.h"
#include "commands.h"
#include "spi_lib.h"
#include "timer_lib.h"

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

void xbee_init(void)
{
    // src_port = get configuration from EEPROM TODO
    // dest_port = get configuration from EEPROM TODO

    // SPI CONFIGURATION
    SPI_Config_t spi_config;
    spi_config.cpol_ = 0;
    spi_config.cpha_ = 0;
    spi_config.mode_ = 0;
    spi_config.bitRate_ = 9600;
    spi_config.dataWidth_ = 8;

    SPI_init(SPI_CHANNEL, spi_config);
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

    //TODO: fix size/length
    //SPI_send_buffer(SPI_CHANNEL, buf, length + 4);

    uint8_t message[20] = { 0x7E, 0x00, 0x10, 0x20, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x26, 0x16, 0x26, 0x16, 0x00, 0x00, 0x41, 0x4C, 0x4C, 0x4F, 0x43 }; //ALLO
    SPI_send_buffer(SPI_CHANNEL, message, 20);

}

void SPI_send_buffer(uint8_t channel, uint8_t* buffer, uint8_t buffer_size)
{
    int i;
    for (i = 0; i < buffer_size; i++)
    {
        SPI_send8Bits(channel, buffer[i]);
    }
}
