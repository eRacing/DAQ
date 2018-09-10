/*
 * can_lib.h
 *
 *  Created on: Jun 18, 2018
 *      Author: Felix Collin
 */

#ifndef CAN_LIB_H_
#define CAN_LIB_H_

#include "globals_lib.h"
#include "buffer.h"

#define RX_MSG_OBJ 1

#define CAN_BITRATE 500000

#define CAN_BUFFER_SIZE 512

#define TIMEOUT_LOG_CAN 1000

typedef struct{
    //
    //! The CAN message identifier used for 11 or 29 bit identifiers.
    //
    uint32_t ui32MsgTimeStamp;

    //
    //! The CAN message number
    //
    uint32_t ui32MsgNumber;

    //
    //! The CAN message identifier used for 11 or 29 bit identifiers.
    //
    uint16_t ui32MsgID;

    //
    //! This value is the number of bytes of data in the message object.
    //
    uint8_t ui32MsgLen;

    //
    //! This is a pointer to the message object's data.
    //
    uint8_t pui8MsgData[8];
}msgEntry;

extern volatile struct circular canmessages;
extern msgEntry sCANMessagesBuffer[CAN_BUFFER_SIZE];
extern volatile uint32_t canBufferPosition;
extern volatile uint32_t ticksCan;

void InitCan(void);

#endif /* CAN_LIB_H_ */
