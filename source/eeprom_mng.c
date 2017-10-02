#include "can_lib.h"
#include "eeprom_mng.h"

#define ADDR_CAN_BITRATE	0		// Configured bitrate
#define ADDR_UDP_PORT		4		// Configured port for transmitting
#define ADDR_BROADCAST_IP	8		// Configured IP adress for transmitting

// Read configuration from EEPROM
CAN_Speed_t read_CAN_bitrate_config(void)
{
    uint8_t rate;

    EEPROM_read_8bits(ADDR_CAN_BITRATE, &rate);

    if (rate == 0xFF)
        return BITRATE_500000; // Default (eeprom empty)

    return (CAN_Speed_t) rate;
}

// Write configuration to eeprom
void write_CAN_bitrate_config(CAN_Speed_t speed)
{
    uint8_t rate = (uint8_t) speed;

    EEPROM_write_8bits(ADDR_CAN_BITRATE, rate);
}

// Read configuration from EEPROM
uint16_t read_UDP_port_config(void)
{
    uint16_t port;

    EEPROM_read_16bits(ADDR_UDP_PORT, &port);

    if (port == 0xFF)
        return 50000; // Default

    return port;
}

// Write configuration to eeprom
void write_UDP_port_config(uint16_t port)
{
    EEPROM_write_16bits(ADDR_UDP_PORT, port);
}
