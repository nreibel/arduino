#include "os.h"
#include "timer.h"

#if TARGET == LINUX_x86
#include "stdio.h"
#include "unistd.h"
#endif

/* Dummy cyclic function */
void Os_Cyclic500ms(void)
{
#if TARGET == LINUX_x86
	printf("hello\n");
#endif
}

/* Project-specific initialization */
void Os_Cfg_Init()
{
	Timer_Init(Timer_MainTask, 500, &Os_Cyclic500ms);
}

/* Wait for a millisecond. Implementation depends on the platform */
void Os_Cfg_Sleep1ms(void)
{
#if TARGET == LINUX_x86
	usleep(1000);
#endif
}
