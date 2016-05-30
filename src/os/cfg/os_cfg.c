#include "os.h"
#include "timer.h"
#include "bits.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "app.h"
#include "serial.h"
#include "eeprom.h"

static volatile uint32_t currentTimeMs = 0;

ISR(TIMER2_COMPA_vect)
{
	currentTimeMs++;
}

uint32_t Os_GetCurrentTimeMs()
{
	return currentTimeMs;
}

void Os_Sleep()
{
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_cpu();

	/*
	 * CPU is sleeping...
	 */

	sleep_disable();
}

void Os_Init()
{
	// Disable all peripherals
	PRR = 0xFF;

	/* Init Timer2 as 1ms counter with interrupts */
	RESET_BIT(PRR, PRTIM2);   // Enable peripheral
	OCR2A  = (F_CPU/64)/1000; // Count 1000ms
	TCNT2  = 0;               // Reset timer value
	TCCR2A = 0x2;             // CTC mode
	TCCR2B = 0x4;             // Set prescaler to 64
	SET_BIT(TIMSK2, OCIE2A);  // Enable interrupt on Compare Match A

	// Enable interrupts
	sei();

	/* Initialization of the application */
	App_Init();
}

Std_ReturnType Os_ExecuteBackgroundTasks()
{
	Std_ReturnType retval = Status_OK;

	if (Serial_BackgroundTask() == Status_Pending)
	{
		retval = Status_Pending;
	}

	if (EEPROM_BackgroundTask() == Status_Pending)
	{
		retval = Status_Pending;
	}

	return retval;
}
