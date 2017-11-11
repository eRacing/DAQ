//
//	CAN manager
//

#ifndef CAN_MNG_H
#define CAN_MNG_H

#include "can_lib.h"

typedef struct
{
    CAN_message_t msg;
    uint32_t timestamp;
    uint32_t number;

} CAN_log_message_t;

#endif // CAN_MNG_H
