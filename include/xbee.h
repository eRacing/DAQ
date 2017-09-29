//
//	Xbee manager
//

#include "can_mng.h"


//! Initialize xbee module
void xbee_init(void);

//! Send CAN message to wireless interface
void xbee_send_CAN_message(CAN_log_message_t msg);

//Delay fonction //TO DO:Put it in the place
void delay(uint32_t milliseconds);
