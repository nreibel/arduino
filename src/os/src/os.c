#include "os.h"

#include "app.h"
#include "timer_prv.h"
#include "types.h"
#include "os_cfg.h"
#include "avr/io.h"

uint32 Os_GetCurrentTimeMs()
{
	// Call user function
	return Os_Cfg_GetTimeElapsed();
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
		Timer_CyclicTask();
	}

	return 1;
}
