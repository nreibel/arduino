#include "os.h"
#include "timer.h"
#include "bits.h"
#include "avr/io.h"
#include "avr/interrupt.h"
#include "app.h"
#include "serial.h"
#include "eeprom.h"

static volatile uint32_t currentTimeMs = 0;

#if ARCHITECTURE == ARCH_ATTINY84
ISR(TIM0_COMPA_vect)
#elif ARCHITECTURE == ARCH_ATMEGA328
ISR(TIMER2_COMPA_vect)
#endif
{
	currentTimeMs++;
}

uint32_t Os_GetCurrentTimeMs()
{
	return currentTimeMs;
}

void Os_EnableInterrupts()
{
	asm("sei");
}

void Os_Init()
{
#if ARCHITECTURE == ARCH_ATTINY84
	/* Init Timer0 as 1ms counter with interrupts */
	OCR0A  = 250;            // Count to 250 ticks
	TCNT0  = 0;              // Reset timer value
	TCCR0A = 0x2;            // CTC mode
	TCCR0B = 0x4;            // Set prescaler to 64 = 250000 ticks/s
	SET_BIT(TIMSK0, OCIE0A); // Enable interrupt on Compare Match A
#elif ARCHITECTURE == ARCH_ATMEGA328
	/* Init Timer2 as 1ms counter with interrupts */
	OCR2A  = 250;            // Count to 250 ticks
	TCNT2  = 0;              // Reset timer value
	TCCR2A = 0x2;            // CTC mode
	TCCR2B = 0x4;            // Set prescaler to 64 = 250000 ticks/s
	SET_BIT(TIMSK2, OCIE2A); // Enable interrupt on Compare Match A
#endif

	Os_EnableInterrupts();

	/* Initialization of the application */
	App_Init();
}

void Os_ExecuteBackgroundTasks()
{
#if ARCHITECTURE == ARCH_ATMEGA328
	Serial_BackgroundTask();
#endif

	EEPROM_BackgroundTask();
}
