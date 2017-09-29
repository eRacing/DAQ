#include "system_lib.h"
#include "uart_lib.h"
#include "can_lib.h"
#include "timer_lib.h"
#include "globals.h"
#include "xbee.h"
#include "can_mng.h"
#include "eeprom_mng.h"
#include "command_mng.h"

#define CAN_BUS 0 
#define CAN_BITRATE BITRATE_500000
#define SPI_CHANNEL 0

void xbee_send_UDP_packet(uint8_t *payload, uint8_t size);

int main(void) {

    // Initialize Clock and Timer
    Clock_speed_t sys_config = {MAXIMUM};
    sys_config_clock(sys_config);
    timer_init();

    // Initialize CAN
    CAN_Config_t can_config = {CAN_BITRATE, 0, OVERWRITE_OLDEST_MESSAGE};
    CAN_init(CAN_BUS, can_config);

    // Initialize EEPROM

    // Configure Xbee module
    xbee_init();

    uint32_t msg_number = 0;
    uint32_t timestamp = 0;

    CAN_message_t rx_msg={0,0x8,0};
    CAN_log_message_t logmsg={rx_msg,0xFFFFFFFF,0};
		
		uint8_t payload[]="HELLO WORLD";
		
    int i;
    while (1) {
			
//	xbee_send_UDP_packet(payload, 11);
//	 delay(100);


        //cmd_process_command();

        /*
        CAN_read_message(CAN_BUS, &rx_msg);

        logmsg.msg.id = rx_msg.id;
        logmsg.msg.length = rx_msg.length;
        for (i=0; i<rx_msg.length; i++) {
            logmsg.msg.data[i] = rx_msg.data[i];
        }
        logmsg.timestamp = timestamp;
        logmsg.number = msg_number;
        */

        xbee_send_CAN_message(logmsg);

//        msg_number++;
//        timestamp = getTime_ms();

        delay(100);

    }

}

