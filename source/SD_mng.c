
#include "SD_mng.h"

// static some_handler_type current_trace_file

void SD_new_trace(void)
{
	// close file for current_trace_file

	// current_trace_file = create new file

	// Write header (date, bitrate, ...)
}


void SD_write_message(CAN_log_message_t msg)
{
	// append message to current_trace_file
}
