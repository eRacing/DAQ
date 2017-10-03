//
//	CAN manager
//

#ifndef CAN_MNG_H
#define CAN_MNG_H

#include "can_lib.h"

#define CAN_CHANNEL	0

#ifndef FOO_H
#define FOO_H

typedef struct
{
    CAN_message_t msg; 		// Standard CAN message (id, data, dlc)
    uint32_t timestamp;		// Timestamp (in tenth of ms)
    uint32_t number;		// Message number

} CAN_log_message_t;

#endif

// Initialize CAN bus
void init_CAN(void);

// Abstraction second layer function
void send_CAN_message(CAN_message_t msg);

// Reconfigure bit rate of CAN channel
void set_CAN_bitrate(CAN_Speed_t speed);

// Returns the configured CAN bitrate in kbps
CAN_Speed_t get_CAN_bitrate(void);

#endif // CAN_MNG_H
