//
// Command definitions
//

#ifndef COMMANDS_H
#define COMMANDS_H

// Transmission : command + payload
// The command + payload bytes are wrapped in a SLIP (Serial Line Internet Protocol) frame before being transmit
// Packet format (request) : command id (1 byte) | command payload (0 to COMMAND_BUFFER_SIZE - 1 bytes)
// Packet format (response) : command id (1 byte) | command payload (response, multiple bytes, depends on the command)

#define CMD_PING	0x0A
// Ping command, used to see if the device is responding
// Request : CMD_PING (1 byte)
// Response : CMD_PING (1 byte) | firmware revision number (1 byte)

#define CMD_CAN_BITRATE	0x0B
// Set CAN bit rate command
// Request (2 bytes) : CMD_CAN_BITRATE (1 byte) | bitrate (1 byte)
// Response (3 bytes) : CMD_CAN_BITRATE (1 byte) | configured accepted (0 if accepted, 1 if bad bitrate value, 1 byte) | configured bitrate (1 byte)
//
// Supported values for bitrate: 1 (125 kbps), 2 (250 kbps), 3 (500 kbps)

#define CMD_GET_CONFIG	0x0C
// Get module configuration
// Request (1 byte) : CMD_GET_CONFIG (1 byte)
// Response : 
//			[0] : CMD_GET_CONFIG (1 byte)
//			[1] : bitrate code (1 = 125 kbps, 2 = 250 kbps, 3 = 500 kbps)

#define CMD_CAN_RX	0x0D

#endif // COMMANDS_H
