#ifndef INCLUDE_SDCARD_H_
#define INCLUDE_SDCARD_H_

#include <stddef.h>

int sdcardInit();
int sdcardWrite(void* data, size_t len);

#endif
