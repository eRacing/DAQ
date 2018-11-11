/*
 * io_lib.h
 *
 *  Created on: Jun 18, 2018
 *      Author: Felix Collin
 */

#ifndef IO_LIB_H_
#define IO_LIB_H_

#include "buffer.h"
#include "globals_lib.h"

#define LOG_WORD_LENGTH 18
#define LOG_BUFFER_SIZE 512

extern char logBuffer[LOG_BUFFER_SIZE*LOG_WORD_LENGTH];
extern volatile uint32_t logBufferPosition;

extern volatile uint8_t sdPresent;

void InitIo(void);

void InitFile(void);

void writeLogEntries(struct buffer* buffer);

void ToggleLed(void);

#endif /* IO_LIB_H_ */
