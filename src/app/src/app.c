#include "os.h"
#include "app_prv.h"
#include "timer.h"
#include "timer_cfg.h"
#include "serial.h"
#include "serial_prv.h"
#include "port.h"
#include "uss.h"
#include "stdio.h"

static char buffer[20];
static boolean ledOn = FALSE;

void App_Init()
{
	Serial_Init();
	USS_Init();

	Port_SetPinDataDirection(Port_B, Pin_5, Output);

	USS_TriggerMeasurement();

	/* Set up main task */
	Timer_Init(Timer_MainTask, 500, &App_MainCyclic);
}


void App_MainCyclic(void)
{
	uint16_t distance;
	if ( USS_GetDistance(&distance) == Status_OK )
	{
		/* Print distance */
		sprintf(buffer, "%u cm\n\r", distance);
		Serial_Print(buffer);

		/* Trigger next acquisition */
		USS_TriggerMeasurement();
	}

	if (ledOn)
	{
		Port_SetPinState(Port_B, Pin_5, Low);
		ledOn = FALSE;
	}
	else
	{
		Port_SetPinState(Port_B, Pin_5, High);
		ledOn = TRUE;
	}
}
