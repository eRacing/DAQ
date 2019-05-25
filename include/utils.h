#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

#define assert(X) if(!(X)) while(1);

#define SysCtlDelayMS(MS) SysCtlDelay((int) ((((double) SysCtlClockGet()) * ((double) (MS))) / 3000.0f))

#endif
