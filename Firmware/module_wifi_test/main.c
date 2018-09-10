#include "uart_lib.h"
#include "can_lib.h"
#include "timer_lib.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"

#define CAN_BUS 0
#define UART_CHANNEL 1
#define BUFFER_SIZE 21

void delay(uint32_t milliseconds) {
	MAP_SysCtlDelay((MAP_SysCtlClockGet() / 3) * (milliseconds / 1000.0f));
}

void main(void) {

	MAP_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	UART_Config_t uart_config;
	uart_config.bit_rate = BITRATE_115200;
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
	uint8_t buf[BUFFER_SIZE];
	uint32_t message_number = 0;
	uint32_t timestamp;
	uint32_t can_id;

	uint8_t i;

	delay(35000);

	while (1) {
		if (CAN_get_buffer_size(CAN_BUS) > 0) {

			CAN_read_message(CAN_BUS, &can_message);

			// Message number
			message_number++;
			buf[3] = (uint8_t) message_number;
			buf[2] = (uint8_t) (message_number >> 8);
			buf[1] = (uint8_t) (message_number >> 16);
			buf[0] = (uint8_t) (message_number >> 24);

			// Timestamp
			timestamp = getTime_ms()*10; // Tenth of ms
			buf[7] = (uint8_t) timestamp;
			buf[6] = (uint8_t) (timestamp >> 8);
			buf[5] = (uint8_t) (timestamp >> 16);
			buf[4] = (uint8_t) (timestamp >> 24);

			// CAN message ID
			can_id = can_message.id;
			buf[11] = (uint8_t) can_id;
			buf[10] = (uint8_t) (can_id >> 8);
			buf[9] = (uint8_t) (can_id >> 16);
			buf[8] = (uint8_t) (can_id >> 24);

			// DLC
			buf[12] = can_message.length;

			// Data
			for (i=0; i < can_message.length; i++) {
				buf[13+i] = can_message.data[i];
			}

			// Padding with 0
			for (i=13+can_message.length; i<BUFFER_SIZE; i++) {
				buf[i] = 0;
			}

			UART_send_buffer(UART_CHANNEL, buf, BUFFER_SIZE);
		}

	}

}
