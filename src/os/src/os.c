#include "os.h"
#include "os_prv.h"
#include "os_cfg.h"
#include "app.h"
#include "timer.h"

void Os_Wait(uint32_t ms)
{
	uint32_t begin = Os_GetCurrentTimeMs();
	while (Os_GetCurrentTimeMs() < begin + ms);
}

Std_ReturnType Os_ExecuteBackgroundTasks()
{
	Std_ReturnType retval = Status_OK;
	int i = 0;

	for (i = 0 ; i < NUMBER_OF_BACKGROUND_TASKS ; i++)
	{
		if (BackgroundTasksList[i]() == Status_Pending)
		{
			retval = Status_Pending;
		}
	}

	return retval;
}

int main(void)
{
	uint32_t cyclicTime = Os_GetCurrentTimeMs();

	/* Initialize timer */
	Timer_CyclicTaskInit();

	/* Perform project-specific initialization */
	Os_Init();

	/* Initialization of the application */
	App_Init();

	/* Run main loop */
	while(1)
	{
        // Check on cyclic tasks every ms
		if (cyclicTime != Os_GetCurrentTimeMs())
		{
			Timer_CyclicTask();
			cyclicTime = Os_GetCurrentTimeMs();
		}

		// Execute background tasks in the spare time, or sleep until next tick
		if (Os_ExecuteBackgroundTasks() != Status_Pending)
		{
			Os_Sleep();
		}
	}

	return 1;
}
