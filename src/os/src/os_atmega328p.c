#include "os.h"
#include "os_cfg.h"
#include "bits.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

// Current time, to be incremented by 1ms ISR
volatile time_t currentTimeMs = 0;

ISR(TIMER2_COMPA_vect)
{
    currentTimeMs++;
}

time_t Os_GetCurrentTimeMs()
{
    return currentTimeMs;
}

void Os_DisableInterrupts()
{
    cli();
}

void Os_EnableInterrupts()
{
    sei();
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
    RESET_BIT(PRR, PRTIM2);  // Enable peripheral
    SET_BIT(TIMSK2, OCIE2A); // Enable interrupt on Compare Match A
    TCNT2  = 0;              // Reset timer value
    TCCR2A = 0x2;            // CTC mode
    OCR2A  = 250;            // Count 1000ms (works with prescaler)
    TCCR2B = 0x4;            // Set prescaler to 64
}
