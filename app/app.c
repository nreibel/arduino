#include "stdio.h"
#include "os.h"
#include "app.h"
#include "timer.h"
#include "icp.h"
#include "serial.h"

/*
 * Timer demo :
 * - Timer 0 : Output 244Hz PWM at 50% duty cycle on OC0A (D6)
 * - Timer 1 : Input Capture mode on ICP1 (D8)
 * - Timer 2 : OS Timer (1ms counter)
 *
 * Link OC0A (D6) and ICP1 (D8) together to read PWM
 * Result is displayed on serial output
 * Shoud read 244Hz and 0.5 duty cycle
 */

#define BUFFER_SZ 64
char buffer[BUFFER_SZ];

const __flash char STR_READY[] = "Ready!";
const __flash char STR_ERROR[] = "Error %d";
const __flash char STR_NO_INPUT[] = "No input";
const __flash char STR_FREQUENCY[] = "Frequency = %uHz";
const __flash char STR_DUTY_CYCLE[] = "Duty cycle = %.03f";
const __flash char STR_ERROR_TIMER_INIT[] = "INIT TIMER FAILED";
const __flash char STR_ERROR_TIMER_START[] = "START TIMER FAILED";

// App entry point
void app_init()
{
    serial_bus_init(SERIAL_BUS_0, 19200);

    timer_config_t timer_cfg = {
        .mode = TIMER_MODE_FAST_PWM,
        .output_mode_a = TIMER_OUTPUT_MODE_DEFAULT,
        .prescaler = TIMER_PRESCALER_256,
        .ocra = 127,
    };

    if (timer_init(TIMER_0, &timer_cfg) < 0)
    {
        serial_println_p(SERIAL_BUS_0, STR_ERROR_TIMER_INIT);
    }
    else if (timer_start(TIMER_0) < 0)
    {
        serial_println_p(SERIAL_BUS_0, STR_ERROR_TIMER_START);
    }

    icp_init(ICP1, ICP_PRESCALER_64);

    // Init tasks
    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);
    os_timer_start(TIMER_INTERVAL);

    os_watchdog_enable(OS_WATCHDOG_2S);

    serial_println_p(SERIAL_BUS_0, STR_READY);
}

// Main task
int task_main(void* data)
{
    UNUSED(data);

    int ret = 0;
    float dutyCycle = 0;
    uint16_t freq = 0;

    os_watchdog_trigger();

    ret = icp_get_duty_cycle(ICP1, &dutyCycle);
    switch(ret)
    {
        case ICP_OK:
            snprintf_P(buffer, BUFFER_SZ, STR_DUTY_CYCLE, dutyCycle);
            serial_println(SERIAL_BUS_0, buffer);
            break;

        case -ICP_ERROR_NO_DATA:
        case -ICP_ERROR_OVERFLOW:
            serial_println_p(SERIAL_BUS_0, STR_NO_INPUT);
            break;

        default:
            snprintf_P(buffer, BUFFER_SZ, STR_ERROR, ret);
            serial_println(SERIAL_BUS_0, buffer);
            break;
    }

    ret = icp_get_frequency(ICP1, &freq);
    switch(ret)
    {
        case ICP_OK:
            snprintf_P(buffer, BUFFER_SZ, STR_FREQUENCY, freq);
            serial_println(SERIAL_BUS_0, buffer);
            break;

        case -ICP_ERROR_NO_DATA:
        case -ICP_ERROR_OVERFLOW:
            serial_println_p(SERIAL_BUS_0, STR_NO_INPUT);
            break;

        default:
            snprintf_P(buffer, BUFFER_SZ, STR_ERROR, ret);
            serial_println(SERIAL_BUS_0, buffer);
            break;
    }

    return 0;
}
