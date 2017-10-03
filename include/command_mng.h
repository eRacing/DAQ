//
// Command manager
//

#ifndef COMMAND_MNG_H
#define COMMAND_MNG_H

#include "lib_config.h"

// Types
// TODO : take off
//typedef unsigned char uint8_t;
//typedef unsigned short uint6;
//typedef unsigned int unit32;

typedef enum
{
    WIRELESS_SESSION,// A CAN session is going on (receiving / transmitting commands over CAN)
    SERIAL_SESSION,	// A serial session is going on (receiving / transmitting commands over serial communication)
    NO_SESSION		// No session, command interface available
} Cmd_Session_Type_t;

extern Cmd_Session_Type_t cmd_session_type;

// CMD_ERROR : Error command id (sent when an error occured)
// Command Payload : Error code (1 byte) | previous command (1 byte, 0 if it does not apply)
#define CMD_ERROR 0x01

// Error codes :
#define ERR_FRAME_ERROR 		1	//!< Invalid frame detected (ESC character not followed by ESC_ESC or ESC_END)
#define ERR_OVERFLOW			2	//!< Command payload too big for the command buffer
#define ERR_TIMEOUT				3	//!< Session has expired with a pending partial command in the buffer, command buffer has been resetted.
#define ERR_PAYLOAD_SIZE		4	//!< Wrong payload size for the specified command
#define ERR_SESSION_LOCKED		5	//!< There is a session already opened with another communication interface
#define ERR_UNKNOWN_CMD			6	//!< The command is unknown

// Only functions intended to be used in other files are prototyped here
void cmd_bytes_received(uint8_t *data, uint8_t size);
void cmd_byte_received(uint8_t byte);
void cmd_process_command(void);
void cmd_slip_and_send(uint8_t *data, uint8_t size);
void cmd_send_error_code(uint8_t error_code, uint8_t command_id);
void cmd_send_command(uint8_t command_code, uint8_t *payload, uint8_t size);

#endif // COMMAND_MNG_H
