#include "os.h"

/*
 * Private data
 */

static struct {
    time_t value;
    bool running;
} timers[NUMBER_OF_OS_TIMERS];

/*
 * Public functions
 */

void os_timer_start(os_timer_t timer)
{
    timers[timer].value = os_millis();
    timers[timer].running = TRUE;
}

void os_timer_reset(os_timer_t timer)
{
    timers[timer].value = os_millis();
}

void os_timer_stop(os_timer_t timer)
{
    timers[timer].value = os_millis() - timers[timer].value;
    timers[timer].running = FALSE;
}

void os_timer_resume(os_timer_t timer)
{
    timers[timer].value = os_millis() - timers[timer].value;
    timers[timer].running = TRUE;
}

time_t os_timer_value(os_timer_t timer)
{
    if (timers[timer].running) return os_millis() - timers[timer].value;
    else return timers[timer].value;
}
