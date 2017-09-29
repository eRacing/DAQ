
#include "command_mng.h"
#include "commands.h"
#include "uart_lib.h"
#include "globals.h"
#include "can_mng.h"

// Boolean
#define TRUE 1
#define FALSE 0

// SLIP (Serial Line Internet Protocol) special bytes
#define END 0xC0
#define ESC 0xBD
#define ESC_END 0xDC
#define ESC_ESC 0xDD

#define COMMAND_BUFFER_SIZE 30	//!< Command buffer size


static uint8_t buf[COMMAND_BUFFER_SIZE]; // Command buffer
static uint8_t buf_size = 0; // Buffer fill size
static uint8_t command_end = FALSE;	//!< True (equals 1) when all the bytes of a command has been received (end of frame)
static uint8_t command_overflow = FALSE; //!< True (1) when the command + payload is bigger than the command buffer

/*! \brief Indicates the communication interface currently used for transmitting and receiving commands

	The session type will be set when a command is received from a valid communication interface. Once the session
	is set, the command responses will be sent to the correct interface and the other interface will be blocked so
	no commands get in when one interface is already communicating.
*/
Cmd_Session_Type_t cmd_session_type;


// Send a byte to the communication interface
static void cmd_send_byte(uint8_t byte);


//
//	Receive and process functions :
//


/*!	\brief Replaces the SLIP escape combinations by the represented byte. All bytes received must pas trought that function.

	\param[in] byte Next command byte received
	\param[out] unslipped Value is TRUE (1) if the byte return is valild, FALSE (0) if not (need the next byte to convert)
	\param[out] end TRUE (1) if it is the end of the frame, FALSE (0) otherwise
	\param[out] frame_error TRUE (1) if there is a frame error (impossible byte combinasion : ESC not followed by ESC_END of ESC_ESC), FALSE (0) otherwise
*/
static uint8_t unslip(uint8_t byte, uint8_t *unslipped, uint8_t *end, uint8_t *frame_error)
{
	static uint8_t last_byte = 0;
	*end = FALSE;
	*unslipped = FALSE;
	*frame_error = FALSE;

	if(byte == END)
	{
		*end = TRUE; // End of frame
		return 0;
	}

	if(last_byte == ESC)
	{
		if(byte == ESC_END)
		{
			*unslipped = TRUE;
			last_byte = 0;
			return END;
		}
		else if (byte == ESC_ESC)
		{
			*unslipped = TRUE;
			last_byte = 0;
			return ESC;
		}
		else
		{
			*frame_error = TRUE;
			return 0;
		}
	}

	if(byte == ESC)
	{
		last_byte = byte;
		return 0;
	}

	*unslipped = TRUE;
	return byte;
}


/*! \brief Called when some command bytes are received

	This function fills the command buffer after converting SLIP special characters combination to the represented byte.
	This function only receives the command and sets a flag when a complete command is ready in the command buffer. It
	does not process the command itself because it might be called by an interrupt, so we aim to limit the function's
	tasks to the minimum. The cmd_process_command function is used to processe the commands.
*/
void cmd_bytes_received(uint8_t *data, uint8_t size)
{
	uint8_t i, end, unslipped, frame_error, byte;

	// Copy the received bytes to the command buffer
	for(i = 0; i < size; i++)
	{
		end = 0;
		unslipped = 0;
		frame_error = 0;

		byte = unslip(data[i], &unslipped, &end, &frame_error);

		if(unslipped == TRUE)
		{
			if(buf_size >= COMMAND_BUFFER_SIZE)
			{
				// Command payload too long
				command_overflow = TRUE;
				return;
			}

			buf[buf_size] = byte;
			buf_size++;
		}

		if(end == TRUE)
		{
			command_end = TRUE;
			break;
		}
	}
}


//! Shortcut function to cmd_bytes_received for data length of 1 byte
void cmd_byte_received(uint8_t byte)
{
	cmd_bytes_received(&byte, 1);
}


/*!< \brief Process command when end of frame is received

	This function is intended to be called periodically to process the command waiting in the
	command buffer.
*/
void cmd_process_command()
{
	uint8_t command_id;

	if(command_overflow == TRUE) // Command with payload that exceeds the buffer size
	{
		cmd_send_error_code(ERR_OVERFLOW, buf[0]);
		buf_size = 0;
		command_end = FALSE;
		command_overflow = FALSE;

		return;
	}

	if(command_end == FALSE) // Command not received yet
		return;

	if(buf_size < 1)	// Command with less then one byte (not valid)
	{
		// TODO : Send error message
		buf_size = 0;
		command_end  = FALSE;
		return;
    }

	// Command processing

	command_id = buf[0];

	switch(command_id)
	{
		// Implemented commands treated here
		// ex :
		// case GET_SPEED_MEASRUE: // Get speed measure from the controller
		//		command_get_speed(buf, buf_size); // Command processed by the specific command function

		case CMD_PING:
			{
				uint8_t firmware_revision = FIRMWARE_REVISION;
				cmd_send_command(CMD_PING, &firmware_revision, 1); // Send response
			}
			break;

/* CAN_set_bitrate n'est pas complete, la commande ne set à rien tant que la fonction n'est pas debuggée
		case CMD_CAN_BITRATE:
			{
				if(buf_size < 2)
				{
					cmd_send_error_code(ERR_PAYLOAD_SIZE, command_id);
					break;	// Ignore command
				}

				uint8_t rbuf[3]; // Response buffer
				rbuf[0] = CMD_CAN_BITRATE;
				rbuf[1] = 0; // Command success
				rbuf[2] = 0;

				// Set bitrate
				if(buf[1] == 1)
					set_CAN_bitrate(BITRATE_125000);
				else if (buf[1] == 2)
					set_CAN_bitrate(BITRATE_250000);
				else if(buf[1] == 3)
					set_CAN_bitrate(BITRATE_500000);
				else
					rbuf[1] = 1;	// Bad bitrate

				// Bit rate code
				if(get_CAN_bitrate() == BITRATE_125000)
					rbuf[2] = 1;
				if(get_CAN_bitrate() == BITRATE_250000)
					rbuf[2] = 2;
				if(get_CAN_bitrate() == BITRATE_500000)
					rbuf[2] = 3;

				cmd_slip_and_send(rbuf, 3);
			}
			break;
*/

		case CMD_GET_CONFIG:
			{
				uint8_t rbuf[2];
				rbuf[0] = CMD_GET_CONFIG;

				// Bit rate code
				if(get_CAN_bitrate() == BITRATE_125000)
					rbuf[1] = 1;
				if(get_CAN_bitrate() == BITRATE_250000)
					rbuf[1] = 2;
				if(get_CAN_bitrate() == BITRATE_500000)
					rbuf[1] = 3;

				cmd_slip_and_send(rbuf, 2);
			}
			break;

		default: // Unknown command
			cmd_send_error_code(ERR_UNKNOWN_CMD, command_id);
			break;
	}


	// Reset buffer size and end of frame indicator

	buf_size = 0;
	command_end = FALSE;

}


//
//	Transmit functions :
//


void cmd_send_data(uint8_t *data, uint8_t size)
{
	// Depends on the interfaces available, uses library functions if possible
	uint8_t i;
	for(i = 0; i < size; i++)
	{
		UART_send_data(0, data[i]);
	}

	if(cmd_session_type == WIRELESS_SESSION)
	{
		// TODO
		//... use CAN functions to send responses
	}
	else if(cmd_session_type == SERIAL_SESSION)
	{
		// TODO
		//... use serial com functions to send responses
	}
}

//! Shortcut to cmd_send_data for lengh of one byte
void cmd_send_byte(uint8_t byte)
{
	cmd_send_data(&byte, 1);
}

//! Send the specified data buffer in a SLIP frame
void cmd_slip_and_send(uint8_t *data, uint8_t size)
{
	// SLIP the command before sending
	uint8_t i;

	for(i = 0; i < size; i++)
	{
		if(data[i] == ESC)
		{
			// Replace by ESC + ESC_ESC
			cmd_send_byte(ESC);
			cmd_send_byte(ESC_ESC);
		}
		else if(data[i] == END)
		{
			// Replace by ESC + ESC_END
			cmd_send_byte(ESC);
			cmd_send_byte(ESC_END);
		}
		else
			cmd_send_byte(data[i]);
	}

	// Send END byte
	cmd_send_byte(END);
}

void cmd_send_error_code(uint8_t error_code, uint8_t command_id)
{
	uint8_t data[3];
	data[0] = CMD_ERROR;
	data[1] = error_code;
	data[2] = command_id;

	cmd_slip_and_send(data, 3);
}

void cmd_send_command(uint8_t command_code, uint8_t *payload, uint8_t size)
{
	uint8_t buffer[100];
	uint8_t i;

	if(size > 100)
		return;

	buffer[0] = command_code;

	for(i = 0; i < size; i++)
	{
		buffer[i + 1] = payload[i];
	}

	cmd_slip_and_send(buffer, size + 1);
}



//
//	Command specific function
//


