#include "uart_lib.h"
#include "can_lib.h"
#include "timer_lib.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"

#define CAN_BUS 0
#define UART_CHANNEL 1

//
// Ensure those values match with Module_Wifi_Receiver
//
#define TRANSMIT_DELAY_MS 5
#define MESSAGES_PER_TRANSMIT 3
#define SINGLE_MESSAGE_SIZE 15

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
	uint8_t buf[(MESSAGES_PER_TRANSMIT * SINGLE_MESSAGE_SIZE)];

	uint8_t i;
	uint8_t j;

	delay_ms(40000);

	/* [TODO] : Debug the module config

	 //
	 // Wifi Module Configuration
	 //

	 // Exit the transparent transmission mode to enter AT Command mode
	 UART_send_data(UART_CHANNEL,'+');
	 UART_send_data(UART_CHANNEL,'+');
	 UART_send_data(UART_CHANNEL,'+');
	 delay_ms(500);
	 UART_send_data(UART_CHANNEL,(uint8_t)0x1B);
	 UART_send_data(UART_CHANNEL,(uint8_t)0x1B);
	 UART_send_data(UART_CHANNEL,(uint8_t)0x1B);

	 // AT Commands
	 UART_send_buffer(UART_CHANNEL, "at+netmode=2\r\n", 14);
	 UART_send_buffer(UART_CHANNEL, "at+dhcpd=0\r\n", 12);
	 UART_send_buffer(UART_CHANNEL, "at+wifi_conf=ERACING_DAQ,wpa2_aes,polytechnique\r\n", 49);
	 UART_send_buffer(UART_CHANNEL, "at+dhcpc=1\r\n", 12);
	 UART_send_buffer(UART_CHANNEL, "at+net_ip=0.0.0.0,0.0.0.0,0.0.0.0\r\n", 35);
	 UART_send_buffer(UART_CHANNEL, "at+net_dns=0.0.0.0,0.0.0.0\r\n", 28);
	 UART_send_buffer(UART_CHANNEL, "at+remotepro=udp\r\n", 18);
	 UART_send_buffer(UART_CHANNEL, "at+mode=client\r\n", 16);
	 UART_send_buffer(UART_CHANNEL, "at+remoteip=192.168.11.245\r\n", 28);
	 UART_send_buffer(UART_CHANNEL, "at+remoteport=8080\r\n", 20);
	 UART_send_buffer(UART_CHANNEL, "at+timeout=0\r\n", 14);
	 UART_send_buffer(UART_CHANNEL, "at+uart=115200,8,n,1\r\n", 22);
	 UART_send_buffer(UART_CHANNEL, "at+uartpacklen=64\r\n", 19);
	 UART_send_buffer(UART_CHANNEL, "at+uartpacktimeout=10\r\n", 23);
	 UART_send_buffer(UART_CHANNEL, "at+net_commit=1\r\n", 17);
	 UART_send_buffer(UART_CHANNEL, "at+reconn=1\r\n", 13);
	 */

	uint32_t timer = 0;

	while (1) {
		if (CAN_get_buffer_size(CAN_BUS) > 2) {
			for (i = 0; i < MESSAGES_PER_TRANSMIT; i++) {
				CAN_read_message(CAN_BUS, &can_message);

				// DLC
				buf[(i * SINGLE_MESSAGE_SIZE)] = can_message.length;

				// Data
				for (j = 1; j < can_message.length+1; j++) {
					buf[j + (i * SINGLE_MESSAGE_SIZE)] = can_message.data[j];
				}

				// Padding with 0
				for (j = can_message.length+1; j < 9; j++) {
					buf[j + (i * SINGLE_MESSAGE_SIZE)] = 0;
				}

				// Message ID
				buf[9 + (i * SINGLE_MESSAGE_SIZE)] = (uint8_t) can_message.id;
				buf[10 + (i * SINGLE_MESSAGE_SIZE)] = (uint8_t) (can_message.id
						>> 8);
				buf[11 + (i * SINGLE_MESSAGE_SIZE)] = (uint8_t) (can_message.id
						>> 16);
				buf[12 + (i * SINGLE_MESSAGE_SIZE)] = (uint8_t) (can_message.id
						>> 24);

				buf[13 + (i * SINGLE_MESSAGE_SIZE)] = '\r';
				buf[14 + (i * SINGLE_MESSAGE_SIZE)] = '\n';
			}
			if ((getTime_ms() - timer) < TRANSMIT_DELAY_MS) {
				delay_ms(TRANSMIT_DELAY_MS - (getTime_ms() - timer));
			}

			UART_send_buffer(UART_CHANNEL, buf,
					(MESSAGES_PER_TRANSMIT * SINGLE_MESSAGE_SIZE));
			timer = getTime_ms();
		}
	}
}
