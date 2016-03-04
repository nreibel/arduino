#include "os.h"

#include "app.h"
#include "timer_prv.h"
#include "os_cfg.h"

extern uint32_t Os_GetCurrentTimeMs();

void Os_EnableInterrupts()
{
	asm("sei");
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

	/* Initialization of the application */
	App_Init();

	/* Run main loop */
	while(1)
	{
		Os_Cfg_ExecuteBackgroundTasks();
		Timer_CyclicTask();
	}

	return 1;
}
