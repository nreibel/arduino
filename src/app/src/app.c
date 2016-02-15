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
	Serial_Print("Hello, world!\n\r");
	if (ledOn)
	{
		RESET_BIT(PORTB, 5);
		ledOn = FALSE;
	}
	else
	{
		SET_BIT(PORTB, 5);
		ledOn = TRUE;
	}
}
