#include "os.h"
#include "os_cfg.h"
#include "bits.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

static volatile time_t currentTimeMs = 0;

ISR(TIMER0_COMPA_vect)
{
    currentTimeMs++;
}

time_t os_millis()
{
    return currentTimeMs;
}

void os_interrupts_disable()
{
    cli();
}

void os_interrupts_enable()
{
    sei();
}

void os_reset()
{
  cli();                 // disable interrupts
  wdt_enable(WDTO_15MS); // enable watchdog
  for(;;){}              // wait for watchdog to reset processor
}

void os_watchdog_trigger()
{
    wdt_reset();
}

int os_watchdog_enable(os_watchdog_t wd)
{
    switch(wd)
    {
        case OS_WATCHDOG_15MS:  wdt_enable(WDTO_15MS);  return 0;
        case OS_WATCHDOG_30MS:  wdt_enable(WDTO_30MS);  return 0;
        case OS_WATCHDOG_60MS:  wdt_enable(WDTO_60MS);  return 0;
        case OS_WATCHDOG_120MS: wdt_enable(WDTO_120MS); return 0;
        case OS_WATCHDOG_250MS: wdt_enable(WDTO_250MS); return 0;
        case OS_WATCHDOG_500MS: wdt_enable(WDTO_500MS); return 0;
        case OS_WATCHDOG_1S:    wdt_enable(WDTO_1S);    return 0;
        case OS_WATCHDOG_2S:    wdt_enable(WDTO_2S);    return 0;
        case OS_WATCHDOG_4S:    wdt_enable(WDTO_4S);    return 0;
        case OS_WATCHDOG_8S:    wdt_enable(WDTO_8S);    return 0;
        default: return -1;
    }
}

void os_sleep()
{
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_cpu();

    /*
    * CPU is sleeping...
    */

    sleep_disable();
}

void os_init()
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
