

#include "can_mng.h"
#include "eeprom_mng.h"

static CAN_Speed_t can_bitrate; // Current bitrate

void init_CAN(void)
{
	// CAN configuration
	CAN_Config_t can_config;
	can_config.bit_rate = read_CAN_bitrate_config();
	can_config.mask = 0;	// No filter
	can_config.buffer_behaviour = OVERWRITE_OLDEST_MESSAGE;
	CAN_init(CAN_CHANNEL, can_config);
}

void send_CAN_message(CAN_message_t msg)
{
	// Send message to the CAN channel
	CAN_send_message(CAN_CHANNEL, msg);
}

void set_CAN_bitrate(CAN_Speed_t speed)
{
	if(speed == can_bitrate)
		return; // Already this speed

	// Set bitrate for the CAN channel
	CAN_set_bitrate(CAN_CHANNEL, speed);
	can_bitrate = speed;
}

CAN_Speed_t get_CAN_bitrate(void)
{
	return can_bitrate;
}
