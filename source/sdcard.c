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
static FIL logfile;

#define PREFIX_FILE "LOG"
#define PREFIX_SIZE 3

#define FILENAME_MAX_LENGTH 20

///////////////////////
// Private Functions //
///////////////////////

static int findNextLogFile() {
    FILINFO fileInfo;
    FRESULT result;
    DIR directory;
    int logMaxNumber = 0;

    result = f_findfirst(&directory, &fileInfo, "/", PREFIX_FILE "*");
    if(result != FR_OK) goto fatfs_error;

    while(fileInfo.fname[0] != '\0') {
        result = f_findnext(&directory, &fileInfo);
        if(result != FR_OK) goto close_dir;

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

    result = f_closedir(&directory);
    if(result != FR_OK) goto fatfs_error;

    return logMaxNumber+1;

close_dir:
    f_closedir(&directory);
fatfs_error:
    return result;
}

static int openLogFile(int logNumber, FIL* logFile) {
    char filename[FILENAME_MAX_LENGTH];
    if(snprintf(filename, FILENAME_MAX_LENGTH, "/" PREFIX_FILE "%d", logNumber) < 0) {
        return -1;
    }

    FRESULT result = f_open(logFile, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if(result != FR_OK) {
        return -1;
    }

    return 0;
}

static int remount(FATFS* filesystem) {
    FRESULT result;

    result = f_mount(0, "", 0);
    if(result != FR_OK) {
        return -1;
    }

    result = f_mount(filesystem, "", 1);
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

    int logfileNumber = findNextLogFile();
    if(logfileNumber < 0) {
        return -1;
    }

    /*
     * For some reason, fatfs refuses to open the log file after
     * the search done by findNextLogFile(). We fix this by
     * remounting the filesystem.
     */
    if(remount(&filesystem) < 0) {
        return -1;
    }

    if(openLogFile(logfileNumber, &logfile) < 0) {
        return -1;
    }

    return 0;
}

int sdcardWrite(void* data, size_t len) {
    size_t written;
    FRESULT result;

    result = f_write(&logfile, data, len, &written);
    if(result != FR_OK) {
        return -1;
    }

    result = f_sync(&logfile);
    if(result != FR_OK) {
         return -1;
    }

    return 0;
}
