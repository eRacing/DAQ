#include "system_lib.h"
#include "timer_lib.h"
#include "can_lib.h"
#include "can_mng.h"
#include "xbee.h"

#define CAN_BUS 0

int main(void)
{

    // Initialize Clock and Timer
    Clock_speed_t sys_config = { MAXIMUM };
    sys_config_clock(sys_config);
    timer_init();

    // Initialize CAN
    CAN_Config_t can_config;
    can_config.bit_rate = BITRATE_500000;
    can_config.mask = 0;
    can_config.buffer_behaviour = OVERWRITE_OLDEST_MESSAGE;
    can_config.pin_selection = 0;
    CAN_init(CAN_BUS, can_config);

    // Initialize EEPROM
    // TODO

    // Initialize XBee module
    delay_ms(5000);
    xbee_init();

    CAN_message_t rx_msg;
    CAN_log_message_t log_msg;
    uint32_t msg_number = 0;
    uint8_t i;

    while (1)
    {

        while (CAN_get_buffer_size(CAN_BUS) > 0) {
            // Read CAN message from bus
            CAN_read_message(CAN_BUS, &rx_msg);

            // Create CAN log message
            log_msg.msg.id = rx_msg.id;
            log_msg.msg.length = rx_msg.length;
            for (i=0; i<rx_msg.length; i++) {
                log_msg.msg.data[i] = rx_msg.data[i];
            }
            log_msg.timestamp = getTime_ms();
            log_msg.number = msg_number;

            // Send CAN log message to XBee
            xbee_send_CAN_message(log_msg);

            msg_number++;
        }

        // Test message
//        CAN_message_t rx_msg_test = { 42, 4, {'a','b','c','d'} };
//        CAN_log_message_t logmsg_test = { rx_msg_test, 341245, msg_number };
//        xbee_send_CAN_message(logmsg_test);
//        msg_number++;
//        delay_ms(100);
    }

}
