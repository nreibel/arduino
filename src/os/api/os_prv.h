#ifndef __OS_PRV_H__
#define __OS_PRV_H__

#include "types.h"

void Os_Init();
void Os_Sleep();
void Os_EnableInterrupts();
void Os_DisableInterrupts();
void Os_TimerCallback();
Std_ReturnType Os_ExecuteBackgroundTasks();

#endif /* __OS_PRV_H__ */
