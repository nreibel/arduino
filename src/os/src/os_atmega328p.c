#include "os.h"
#include "os_cfg.h"
#include "os_prv.h"
#include "bits.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

ISR(TIMER2_COMPA_vect)
{
    Os_TimerCallback();
}

void Os_DisableInterrupts()
{
    cli();
}

void Os_EnableInterrupts()
{
    sei();
}

void Os_HardReset()
{
  cli(); // disable interrupts
  wdt_enable(WDTO_15MS); // enable watchdog
  HALT; // wait for watchdog to reset processor
}

void Os_ResetWatchdog()
{
    wdt_reset();
}

int Os_EnableWatchdog(Os_Watchdog wd)
{
    switch(wd)
    {
        case OS_WATCHDOG_15MS:
            wdt_enable(WDTO_15MS);
            return 0;

        case OS_WATCHDOG_30MS:
            wdt_enable(WDTO_30MS);
            return 0;

        case OS_WATCHDOG_60MS:
            wdt_enable(WDTO_60MS);
            return 0;

        case OS_WATCHDOG_120MS:
            wdt_enable(WDTO_120MS);
            return 0;

        case OS_WATCHDOG_250MS:
            wdt_enable(WDTO_250MS);
            return 0;

        case OS_WATCHDOG_500MS:
            wdt_enable(WDTO_500MS);
            return 0;

        case OS_WATCHDOG_1S:
            wdt_enable(WDTO_1S);
            return 0;

        case OS_WATCHDOG_2S:
            wdt_enable(WDTO_2S);
            return 0;

        case OS_WATCHDOG_4S:
            wdt_enable(WDTO_4S);
            return 0;

        case OS_WATCHDOG_8S:
            wdt_enable(WDTO_8S);
            return 0;
    }

    return -1;
}


void Os_Sleep()
{
    set_sleep_mode(OS_SLEEP_MODE);
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

    // Init Timer2 as CTC counter with interrupts
    RESET_BIT(PRR, PRTIM2);  // Enable peripheral
    SET_BIT(TIMSK2, OCIE2A); // Enable interrupt on Compare Match A
    TCNT2  = 0;              // Reset timer value
    TCCR2A = 0x2;            // CTC mode

// TODO : OCR2A = ticks - 1 ??

#if F_CPU == 16000000
    #if OS_TIMER_GRANULARITY == 16
        OCR2A  = 250;            // Count 250 ticks
        TCCR2B = 0x7;            // Set prescaler to 1024
    #elif OS_TIMER_GRANULARITY == 8
        OCR2A  = 125;            // Count 125 ticks
        TCCR2B = 0x7;            // Set prescaler to 1024
    #elif OS_TIMER_GRANULARITY == 4
        OCR2A  = 250;            // Count 125 ticks
        TCCR2B = 0x6;            // Set prescaler to 256
    #elif OS_TIMER_GRANULARITY == 2
        OCR2A  = 125;            // Count 125 ticks
        TCCR2B = 0x6;            // Set prescaler to 256
    #elif OS_TIMER_GRANULARITY == 1
        OCR2A  = 125;            // Count 125 ticks
        TCCR2B = 0x5;            // Set prescaler to 128
    #else
        #error OS_TIMER_GRANULARITY value is not supported
    #endif
#else
    #error F_CPU value is not supported
#endif
}
