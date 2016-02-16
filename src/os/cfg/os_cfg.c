#include "os.h"
#include "timer.h"
#include "serial_prv.h"
#include "avr/io.h"
#include "avr/interrupt.h"

static volatile uint32_t currentTimeMs = 0;

ISR(TIMER0_COMPA_vect)
{
	currentTimeMs++;
}

uint32_t Os_GetCurrentTimeMs()
{
	return currentTimeMs;
}

void Os_Cfg_Init()
{
	/* Init Timer0 as 1ms counter with interrupts */
	TCCR0B = 0x0;  // Stop timer
	TCCR0A = 0x2;  // CTC mode
	TIMSK0 = 0x2;  // Enable CTC interrupt
	OCR0A  = 250;  // Count to 250 ticks
	TCNT0  = 0;    // Reset timer value
	TCCR0B = 0x3;  // Set prescaler to 64 = 250000 ticks/s
	Os_EnableInterrupts();
}

void Os_Cfg_ExecuteBackgroundTasks()
{
	Serial_BackgroundTask();
}
