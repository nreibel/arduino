#include "os.h"
#include "os_prv.h"
#include "app.h"
#include "timer.h"

void Os_Wait(uint32_t ms)
{
	uint32_t begin = Os_GetCurrentTimeMs();
	while (Os_GetCurrentTimeMs() < begin + ms);
}

int main(void)
{
	uint32_t cyclicTime = Os_GetCurrentTimeMs();

	/* Initialize timer */
	Timer_CyclicTaskInit();

	/* Perform project-specific initialization */
	Os_Init();

	/* Run main loop */
	while(1)
	{
		if (cyclicTime != Os_GetCurrentTimeMs())
		{
			Timer_CyclicTask();
			cyclicTime = Os_GetCurrentTimeMs();
		}

		if (Os_ExecuteBackgroundTasks() != Status_Pending)
		{
			Os_Sleep();
		}
	}

	return 1;
}
