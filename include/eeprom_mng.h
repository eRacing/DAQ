//
//	EEPROM related functions
//

#include "eeprom_lib.h"
#include "can_lib.h"

CAN_Speed_t read_CAN_bitrate_config(void);

void write_CAN_bitrate_config(CAN_Speed_t speed);

uint16_t read_UDP_port_config(void);

void write_UDP_port_config(uint16_t port);
