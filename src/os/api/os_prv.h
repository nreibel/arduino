#ifndef SRC_OS_API_OS_PRV_H_
#define SRC_OS_API_OS_PRV_H_

#include "types.h"

extern void           Os_Init();
extern Std_ReturnType Os_ExecuteBackgroundTasks();
extern void           Os_Sleep();

#endif /* SRC_OS_API_OS_PRV_H_ */
