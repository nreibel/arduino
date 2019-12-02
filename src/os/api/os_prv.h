#ifndef SRC_OS_API_OS_PRV_H_
#define SRC_OS_API_OS_PRV_H_

#include "types.h"
#include "os_cfg.h"

extern BackgroundTask BackgroundTasksList[NUMBER_OF_BACKGROUND_TASKS];

Std_ReturnType Os_ExecuteBackgroundTasks();

#endif /* SRC_OS_API_OS_PRV_H_ */
