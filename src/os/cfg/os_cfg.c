#include "timer.h"
#include "stdio.h"
#include <unistd.h>

/* Dummy cyclic function */
void Os_Cyclic500ms(void)
{
  printf("hello\n");
}

/* Project-specific initialization */
void Os_Cfg_Init()
{
	Timer_Init(Timer_MainTask, 500, &Os_Cyclic500ms);
}

/* Wait for a millisecond. Implementation depends on the platform */
void Os_Cfg_Sleep1ms(void)
{
	usleep(1000);
}
