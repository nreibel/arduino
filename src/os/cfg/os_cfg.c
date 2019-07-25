#include "os.h"
#include "os_cfg.h"
#include "timer.h"
#include "bits.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "app.h"
#include "serial.h"
#include "eeprom.h"

// Current time, to be incremented by 1ms ISR triggered by Timer2
static volatile uint32_t currentTimeMs = 0;

BackgroundTask BackgroundTasksList[] =
{
	Serial_BackgroundTask,
	// EEPROM_BackgroundTask,
};

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

	// Enable pullup resistor on all inputs
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;

	// Init Timer2 as 1ms counter with interrupts
	RESET_BIT(PRR, PRTIM2);   // Enable peripheral
	TCNT2  = 0;               // Reset timer value
	TCCR2A = 0x2;             // CTC mode
	SET_BIT(TIMSK2, OCIE2A);  // Enable interrupt on Compare Match A
	OCR2A  = 250;             // Count 1000ms (works with prescaler)

#if defined F_CPU && F_CPU == F_CPU_16MHZ
	TCCR2B = 0x4;   // Set prescaler to 64
#elif defined F_CPU && F_CPU == F_CPU_2MHZ
	TCCR2B = 0x2;   // Set prescaler to 8
#else
    #error F_CPU is not defined
#endif
    
	// Enable interrupts
	sei();
}
