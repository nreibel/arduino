#include "timer.h"
#include "timer_cfg.h"
#include "avr/io.h"
#include "bits.h"

void App_MainCyclic(void);

void App_Init()
{
	Timer_Init(Timer_MainTask, 100, &App_MainCyclic);

	// Set PB as output
	SET_BIT(DDRB, 5);
}

boolean ledOn = FALSE;

void App_MainCyclic(void)
{
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
