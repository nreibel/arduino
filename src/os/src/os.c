#include "os.h"
#include "os_cfg.h"
#include "bits.h"
#include "types.h"
#include "timer_ll.h"
#include "gpio_ll.h"

#include <util/delay.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#if OS_ENABLE_PRINTF
#include <stdio.h>
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

void os_systick()
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

#ifdef OS_RANDOM_SEED
uint8_t os_rand()
{
    static uint8_t rand = OS_RANDOM_SEED;
    rand ^= os_timer & 0xFF;
    uint8_t lsb = rand & 0x1;
    rand >>= 1;
    if (lsb) rand ^= 0xB8;
    return rand;
}
#endif // OS_RANDOM_SEED

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

void os_wait_us(unsigned int us)
{
    _delay_us(us);
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

    /* Set all GPIO as inputs with pullup enabled to save power */
    for (port_t p = 0 ; p < NUMBER_OF_PORTS ; p++)
    {
        gpio_ll_set_port_data_direction(p, 0x00);
        gpio_ll_set_port_value(p, 0xFF);
    }

    // Configure 1ms system timer
    timer_ll_power_enable(TIMER0);
    timer_ll_set_wgm(TIMER0, TIMER_LL_WGM_CTC);
    timer_ll_set_prescaler(TIMER0, TIMER0_LL_CS_DIV64);
    timer_ll_set_ocra(TIMER0, 250);
    timer_ll_set_imask(TIMER0, TIMER_LL_INT_OCM_A);

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
