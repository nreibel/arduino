#include "timer.h"
#include "timer_cfg.h"
#include "avr/io.h"
#include "bits.h"
#include "app_prv.h"
#include "serial.h"

static boolean ledOn = FALSE;

void App_Init()
{
	Timer_Init(Timer_MainTask, 500, &App_MainCyclic);

	Serial_Init();

	// Set PB as output
	SET_BIT(DDRB, 5);
}


void App_MainCyclic(void)
{

	if (ledOn)
	{
		Serial_Print("Setting LED to OFF!\n\r");
		RESET_BIT(PORTB, 5);
		ledOn = FALSE;
	}
	else
	{
		Serial_Print("Setting LED to ON!\n\r");
		SET_BIT(PORTB, 5);
		ledOn = TRUE;
	}
}
