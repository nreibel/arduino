#ifndef SRC_OS_API_OS_PRV_H_
#define SRC_OS_API_OS_PRV_H_

#include "types.h"

extern void     Os_Init();
extern void     Os_Cfg_EnableInterrupts();
extern uint32_t Os_Cfg_GetCurrentTimeMs();
extern void     Os_ExecuteBackgroundTasks();

#endif /* SRC_OS_API_OS_PRV_H_ */
