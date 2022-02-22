#include "os.h"
#include "os_cfg.h"
#include "bits.h"
#include "types.h"
#include "timer.h"

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#if OS_ENABLE_PRINTF
#include "stdio.h"
#endif // OS_ENABLE_PRINTF

/*
 * Private variables
 */

static volatile time_t os_timer = 0;

/*
 * External functions
 */

void os_cyclic_tasks();
int os_background_tasks();

/*
 * System Timer interrupt
 */

ISR(TIMER0_COMPA_vect)
{
    os_timer++;
}

/*
 * Public functions
 */

void HALT()
{
    // Disable interrupts, watchdog, and all peripherals
    cli();
    wdt_disable();
    power_all_disable();

    // Power down
    for(;;)
    {
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        sleep_cpu();
    }
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

void os_watchdog_trigger()
{
    wdt_reset();
}

time_t os_millis()
{
    return os_timer;
}

void os_sleep()
{
    set_sleep_mode(OS_SLEEP_MODE);
    sleep_enable();
    sleep_cpu();

    /*
    * CPU is sleeping...
    */

    sleep_disable();
}

void os_wait(time_t ms)
{
    if (ms <= 0) return;

    const time_t start = os_timer;

    while(1)
    {
        time_t elapsed = os_timer - start;
        if (elapsed >= ms ) return;
        else os_sleep();
    }
}

int main(void)
{
    /* Disable all peripherals */
    os_interrupts_disable();
    power_all_disable();

    /* Enable pullup resistor on all inputs */
    DDRB = 0;
    DDRC = 0;
    DDRD = 0;
    PORTB = 0xFF;
    PORTC = 0xFF;
    PORTD = 0xFF;

    // Configure 1ms system timer
    timer_config_t timer_cfg = {
        .mode = TIMER_MODE_CTC,
        .imask = BIT(TIMER_INTERRUPT_OCM_A),
        .prescaler = TIMER_PRESCALER_64,
        .ocra = 250
    };

    if (timer_init(TIMER[0], &timer_cfg) < 0)
        HALT();

    if (timer_start(TIMER[0]) < 0)
        HALT();

#if OS_ENABLE_PRINTF
    static FILE os_stdout = FDEV_SETUP_STREAM(os_putc, NULL_PTR, _FDEV_SETUP_WRITE);
    stdout = &os_stdout;
#endif // OS_ENABLE_PRINTF

    /* Enable interrupts */
    os_interrupts_enable();

    /* Initialization of the application */
    app_init();

    /* Run main loop */
    while(1)
    {
        os_cyclic_tasks();

        // Execute background tasks in the spare time, or sleep until next tick
        if (os_background_tasks() == 0)
        {
            os_sleep();
        }
    }

    return 1;
}
