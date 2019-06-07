#include <sdcard.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <ff.h>

#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>

//////////
// Data //
//////////

static FATFS filesystem;
static DIR directory;
static FIL logfile;

#define PREFIX_FILE "log"
#define PREFIX_SIZE 3

#define FILENAME_MAX_LENGTH 20

///////////////////////
// Private Functions //
///////////////////////

static int findNextLogFile(DIR* directory) {
    FILINFO fileInfo;
    FRESULT result;

    result = f_findfirst(directory, &fileInfo, "/", PREFIX_FILE "*");
    if(result != FR_OK) return -1;

    int logMaxNumber = 0;
    while(fileInfo.fname[0] != '\0') {
        result = f_findnext(directory, &fileInfo);
        if(result != FR_OK) return -1;

        int logNumber;
        if(sscanf(fileInfo.fname, PREFIX_FILE "%d", &logNumber) != 1) {
            continue;
        }

        if(logNumber > logMaxNumber) {
            logMaxNumber = logNumber;
        }
    }

    if(logMaxNumber < 0) {
        logMaxNumber = -logMaxNumber;
    }

    return logMaxNumber;
}

static int openLogFile(int logNumber, FIL* logFile) {
    char filename[FILENAME_MAX_LENGTH];
    if(snprintf(filename, FILENAME_MAX_LENGTH, PREFIX_FILE "%d", logNumber) < 0) {
        return -1;
    }

    FRESULT result = f_open(logFile, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if(result != FR_OK) {
        return -1;
    }

    return 0;
}

////////////////
// Public API //
////////////////

int sdcardInit() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_SSI2)));

    FRESULT result = f_mount(&filesystem, "", 1);
    if(result != FR_OK) {
        return -1;
    }

    int logfileNumber = findNextLogFile(&directory);
    if(logfileNumber < 0) {
        return -1;
    }

    if(openLogFile(logfileNumber, &logfile) < 0) {
        return -1;
    }

    return 0;
}

int sdcardWrite(void* data, size_t len) {
    size_t written;
    FRESULT result = f_write(&logfile, data, len, &written);
    if(result != FR_OK) {
        return -1;
    }

    return 0;
}
