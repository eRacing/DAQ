#include "system_lib.h"
#include "can_lib.h"
#include "timer_lib.h"
#include "globals.h"
#include "xbee.h"

#define CAN_BUS 0 
#define CAN_BITRATE BITRATE_500000

int main(void)
{

    // Initialize Clock and Timer
    Clock_speed_t sys_config = { MAXIMUM };
    sys_config_clock(sys_config);
    timer_init();

    // Initialize CAN
    CAN_Config_t can_config = { CAN_BITRATE, 0, OVERWRITE_OLDEST_MESSAGE };
    CAN_init(CAN_BUS, can_config);

    // Initialize EEPROM
    // TODO

    // Initialize XBee module
    xbee_init();


    uint32_t msg_number = 0;
    uint32_t timestamp = getTime_ms();

    while (1)
    {
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

        CAN_message_t rx_msg = { 42, 3, {0x42, 0xAB, 0xCD} };
        CAN_log_message_t logmsg = { rx_msg, timestamp, msg_number };

        xbee_send_CAN_message(logmsg);

        msg_number++;
        timestamp = getTime_ms();

        delay_ms(1000);
    }

}
