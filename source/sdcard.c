#include <ff.h>

static FATFS filesystem;
static DIR directory;
static FILINFO fileInfo;

int sdcardInit() {
    FRESULT result;

    result = f_mount(&filesystem, "", 1);
    if(result != FR_OK) return -1;

    result = f_findfirst(&directory, &fileInfo, "/", "log*");
    if(result != FR_OK) return -1;

    while(fileInfo.fname[0] != '\0') {
        result = f_findnext(&directory, &fileInfo);
        if(result != FR_OK) return -1;

        // TODO: the next N where the file "logN" doesn't exist
    }

    return 0;
}


