#include "os.h"
#include "os_cfg.h"
#include "bits.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

static volatile time_t currentTimeMs = 0;

ISR(TIMER0_COMPA_vect)
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
    PRR0 = 0xFF;
    PRR1 = 0xFF;

    // Enable pullup resistor on all inputs
    DDRB = 0;
    DDRC = 0;
    DDRD = 0;
    PORTB = 0xFF;
    PORTC = 0xFF;
    PORTD = 0xFF;

    // Init Timer0 as 1ms counter with interrupts
    RESET_BIT(PRR0, PRTIM0); // Enable peripheral
    SET_BIT(TIMSK0, OCIE0A); // Enable interrupt on Compare Match A
    TCNT0 =  0;              // Reset timer value
    OCR0A  = 250;            // Count 1000ms (works with prescaler)
    TCCR0A = 0x2;            // CTC mode
    TCCR0B = 0x3;            // Set prescaler to 64
}
