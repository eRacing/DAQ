//
//	Xbee manager
//

#ifndef XBEE_H
#define XBEE_H

#include "can_mng.h"

//! Initialize xbee module
void xbee_init(void);

//! Send CAN message to wireless interface
void xbee_send_CAN_message(CAN_log_message_t msg);

#endif // XBEE_H
