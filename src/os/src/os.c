#include "os.h"
#include "os_prv.h"
#include "app.h"
#include "timer.h"

void Os_EnableInterrupts()
{
	Os_Cfg_EnableInterrupts();
}

uint32_t Os_GetCurrentTimeMs()
{
	return Os_Cfg_GetCurrentTimeMs();
}

void Os_Sleep(uint32_t ms)
{
	uint32_t begin = Os_GetCurrentTimeMs();
	while (Os_GetCurrentTimeMs() < begin + ms);
}

int main(void)
{
	/* Initialize timer */
	Timer_CyclicTaskInit();

	/* Perform project-specific initialization */
	Os_Cfg_Init();

	/* Run main loop */
	while(1)
	{
		Os_Cfg_ExecuteBackgroundTasks();
		Timer_CyclicTask();
	}

	return 1;
}
