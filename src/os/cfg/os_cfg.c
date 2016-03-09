#include "os.h"
#include "timer.h"
#include "bits.h"
#include "avr/io.h"
#include "avr/interrupt.h"

#include "app.h"
#include "app_cfg.h"

#if SERIAL_DEBUG_ENABLED == ON
#include "serial.h"
#endif


#if LCD_ENABLED == ON
#include "lcd.h"
#endif

static volatile uint32_t currentTimeMs = 0;

ISR(TIMER2_COMPA_vect)
{
	currentTimeMs++;
}

uint32_t Os_Cfg_GetCurrentTimeMs()
{
	return currentTimeMs;
}

void Os_Cfg_EnableInterrupts()
{
	asm("sei");
}

void Os_Cfg_Init()
{
	/* Init Timer2 as 1ms counter with interrupts */
	OCR2A  = 250;            // Count to 250 ticks
	TCNT2  = 0;              // Reset timer value
	TCCR2A = 0x2;            // CTC mode
	TCCR2B = 0x4;            // Set prescaler to 64 = 250000 ticks/s
	SET_BIT(TIMSK2, OCIE2A); // Enable interrupt on Compare Match A

	Os_EnableInterrupts();

	/* Initialization of the application */
	App_Init();
}

void Os_Cfg_ExecuteBackgroundTasks()
{
#if SERIAL_DEBUG_ENABLED == ON
	Serial_BackgroundTask();
#endif

#if LCD_ENABLED == ON
	LCD_BackgroundTask();
#endif
}
