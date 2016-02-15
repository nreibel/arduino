#include "os.h"
#include "timer.h"
#include "avr/io.h"
#include "avr/interrupt.h"

static volatile uint32 currentTimeMs = 0;

ISR(TIMER0_COMPA_vect)
{
	currentTimeMs++;
}

uint32 Os_Cfg_GetCurrentTimeMs()
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
	asm("sei");    // Enable global interrupts
}
