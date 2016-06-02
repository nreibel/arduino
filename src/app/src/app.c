#include "os.h"
#include "app.h"
#include "app_prv.h"
#include "app_cfg.h"
#include "timer.h"
#include "port.h"

void App_Init()
{
	Port_SetPinDataDirection(Pin_LED, Output);

	Timer_StartTask(Timer_Blink, LED_BLINK_DELAY, &Task_Blink);
}

// Main task
void Task_Blink(void)
{
	static PinState state = High;

	Port_SetPinState(Pin_LED, state);
	state = (state == Low ? High : Low);
}
