#include "os.h"
#include "timer_prv.h"

int main(void)
{
	/* Initialize timer */
	Timer_CyclicTaskInit();

	/* Perform project-specific initialization */
	Os_Cfg_Init();

	/* Run main loop */
	while(1)
	{
		Timer_CyclicTask1ms();
		Os_Cfg_Sleep1ms();
	}

	return 0;
}
