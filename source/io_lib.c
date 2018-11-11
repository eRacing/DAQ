/*
 * io_lib.c
 *
 *  Created on: Jun 18, 2018
 *      Author: Felix Collin
 */

#include "string.h"
#include "io_lib.h"
#include "can_lib.h"
#include "driverlib/debug.h"
#include "inc/hw_gpio.h"
#include "driverlib/eeprom.h"
#include "timer_lib.h"
#include "ff.h"
#include "diskio.h"
#include "buffer.h"

static FATFS fs;
static FIL file;

static char fileName[32];

volatile uint8_t ledState = 0;

volatile uint8_t sdPresent = 0;

char logBuffer[LOG_BUFFER_SIZE*LOG_WORD_LENGTH];
volatile uint32_t logBufferPosition = 0;

void InitIo(void){
    if(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}
    }

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_0);

    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,ledState);

}

void InitEEPROM(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    while (!(SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0)));

    EEPROMInit();

}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

uint32_t EEPROMNextNumber(void) {

    uint32_t read_data;
    EEPROMRead(&read_data, 0x00, 4);

    read_data++;
    EEPROMProgram(&read_data, 0x00, 4);

    return read_data;
}

void InitFile(void){
    FRESULT result;
    uint32_t fileNumber = 0;

    // Initialize EEPROM
    InitEEPROM();
    fileNumber = EEPROMNextNumber();

    fileName[0] = 'l';
    fileName[1] = 'o';
    fileName[2] = 'g';

    itoa(fileNumber, fileName+3);

    result = f_mount(&fs,"",1);
    result = f_open(&file, fileName, (FA_WRITE | FA_CREATE_ALWAYS));
    f_sync(&file);

    if(result == FR_OK){
        ToggleLed();
        sdPresent = 1;
    }
    else{
        sdPresent = 0;
    }
}

void writeLogEntries(struct buffer* buffer)
{
    if(!sdPresent) return;

    int i = 0;
    for(i = 0; i < ENTRY_COUNT; i++) {
        //for(uint64_t j = 0; j < 1000000; j++);
        unsigned int len;
        f_write(&file, &(buffer->entries[i].data), ENTRY_SIZE, &len);
        f_sync(&file);
    }

/*
    uint32_t i = 0;
    if(canBufferPosition > 0){
        msgEntry sCANMessagesBufferCopy[CAN_BUFFER_SIZE];
        uint32_t canBufferPositionCopy = canBufferPosition;
        for(i = 0;i<canBufferPositionCopy;i++){
            sCANMessagesBufferCopy[i] = sCANMessagesBuffer[i];
        }
        canBufferPosition = 0;

        for(i = 0; i<canBufferPositionCopy; i++)
        {
            uint8_t j = 0;
            logBuffer[logBufferPosition+0] = sCANMessagesBufferCopy[i].ui32MsgTimeStamp & 0xFF;
            logBuffer[logBufferPosition+1] = (sCANMessagesBufferCopy[i].ui32MsgTimeStamp & 0xFF00) >> 8;
            logBuffer[logBufferPosition+2] = (sCANMessagesBufferCopy[i].ui32MsgTimeStamp & 0xFF0000) >> 16;
            logBuffer[logBufferPosition+3] = (sCANMessagesBufferCopy[i].ui32MsgTimeStamp & 0xFF000000) >> 24;
            logBuffer[logBufferPosition+4] = sCANMessagesBufferCopy[i].ui32MsgNumber & 0xFF;
            logBuffer[logBufferPosition+5] = (sCANMessagesBufferCopy[i].ui32MsgNumber & 0xFF00) >> 8;
            logBuffer[logBufferPosition+6] = (sCANMessagesBufferCopy[i].ui32MsgNumber & 0xFF0000) >> 16;
            logBuffer[logBufferPosition+7] = (sCANMessagesBufferCopy[i].ui32MsgNumber & 0xFF000000) >> 24;
            logBuffer[logBufferPosition+8] = sCANMessagesBufferCopy[i].ui32MsgID & 0xFF;
            logBuffer[logBufferPosition+9] = (sCANMessagesBufferCopy[i].ui32MsgID & 0xFF00) >> 8;
            for (j = 0; j < 8; j++)
            {
                if (j < sCANMessagesBufferCopy[i].ui32MsgLen)
                {
                    logBuffer[logBufferPosition+17-j] = sCANMessagesBufferCopy[i].pui8MsgData[j];
                }
                else
                {
                    logBuffer[logBufferPosition+17-j]=0x00;
                }
            }
            logBufferPosition+=LOG_WORD_LENGTH;
        }
    }
    if (sdPresent && ((logBufferPosition == LOG_BUFFER_SIZE*LOG_WORD_LENGTH) || ((GetTickMs()-ticksCan)>TIMEOUT_LOG_CAN && logBufferPosition>0)))
    {
        UINT bw;
        f_write(&file, logBuffer, logBufferPosition, &bw);
        f_sync(&file);
        logBufferPosition = 0;
    }
    else if(!sdPresent)
    {
        logBufferPosition = 0;
    }
*/
}

void ToggleLed(void){
    (ledState==0xFF) ? (ledState = 0) : (ledState = 0xFF);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,ledState);
}
