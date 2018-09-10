#include "uart_lib.h"
#include "can_lib.h"
#include "timer_lib.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"

#define CAN_BUS 0
#define UART_CHANNEL 1

void main(void) {

	MAP_SysCtlClockSet(
	SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	UART_Config_t uart_config;
	uart_config.bit_rate = BITRATE_230400;
	uart_config.data_bits = 8;
	uart_config.stop_bits = 1;
	UART_init(UART_CHANNEL, uart_config);

	CAN_Config_t can_config;
	can_config.bit_rate = BITRATE_500000;
	can_config.mask = 0;
	can_config.buffer_behaviour = OVERWRITE_OLDEST_MESSAGE;
	CAN_init(CAN_BUS, can_config);

	timer_init();

	CAN_message_t can_message;
	uint8_t bufID[4];

	uint8_t i;

	delay_ms(40000);

	while (1) {
		if (UART_get_buffer_size(UART_CHANNEL) > 0) {
			while (UART_read_data(UART_CHANNEL) != '\r') {
			}
			if (UART_read_data(UART_CHANNEL) == '\n') {
				can_message.length = UART_read_data(UART_CHANNEL);

				for (i = 0; i < can_message.length; i++) {
					can_message.data[i] = UART_read_data(UART_CHANNEL);
				}
				for (i = can_message.length; i < 8; i++) {
					can_message.data[i] = 0;
				}

				bufID[0] = UART_read_data(UART_CHANNEL);
				bufID[1] = UART_read_data(UART_CHANNEL);
				bufID[2] = UART_read_data(UART_CHANNEL);
				bufID[3] = UART_read_data(UART_CHANNEL);
				can_message.id = (bufID[3] << 24) | (bufID[2] << 16)
						| (bufID[1] << 8) | (bufID[0]);

				CAN_send_message(CAN_BUS, can_message);
			}
		}
	}
}
