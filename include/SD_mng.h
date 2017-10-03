//
//	SD Card manager
//

#ifndef SD_MNG_H
#define SD_MNG_H

#include "can_mng.h"

//! Start a new trace file
void SD_new_trace(void);

//! Write message to trace
void SD_write_message(CAN_log_message_t msg);

#endif // SD_MNG_H
