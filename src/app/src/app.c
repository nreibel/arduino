#include "os.h"
#include "app_prv.h"
#include "timer.h"
#include "timer_cfg.h"
#include "serial.h"
#include "port.h"

static boolean ledOn = FALSE;

void App_Init()
{
	Timer_Init(Timer_MainTask, 500, &App_MainCyclic);

	Serial_Init();

	Port_SetPinDataDirection(Port_B, Pin_5, Output);
}


void App_MainCyclic(void)
{

	if (ledOn)
	{
		Serial_Print("Setting LED to OFF!\n\r");
		Port_SetPinValue(Port_B, Pin_5, Low);
		ledOn = FALSE;
	}
	else
	{
		Serial_Print("Setting LED to ON!\n\r");
		Port_SetPinValue(Port_B, Pin_5, High);
		ledOn = TRUE;
	}
}
