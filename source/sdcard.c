#include <sdcard.h>
#include <stdbool.h>
#include <stdint.h>
#include <ff.h>

#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>

static FATFS filesystem;
static DIR directory;
static FILINFO fileInfo;

int sdcardInit() {
    /* enable SSI */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    while(!(SysCtlPeripheralReady(SYSCTL_PERIPH_SSI2)));

    /* mount filesystem */
    FRESULT result = f_mount(&filesystem, "", 1);
    if(result != FR_OK) return -1;

    /* find the next while to write data */
    result = f_findfirst(&directory, &fileInfo, "/", "log*");
    if(result != FR_OK) return -1;

    while(fileInfo.fname[0] != '\0') {
        result = f_findnext(&directory, &fileInfo);
        if(result != FR_OK) return -1;

        // TODO: the next N where the file "logN" doesn't exist
    }

    return 0;
}

