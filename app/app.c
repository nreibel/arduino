#include "stdio.h"
#include "os.h"
#include "bits.h"
#include "app.h"
#include "timer.h"
#include "icp.h"
#include "serial.h"

#define BUFFER_SZ 64
char buffer[BUFFER_SZ];

// App entry point
void app_init()
{
    serial_bus_init(SERIAL_BUS_0, 19200);

    timer_config_t timer_cfg = {
        .mode = TIMER_MODE_FAST_PWM,
        .output_mode_a = TIMER_OUTPUT_MODE_DEFAULT,
        .output_mode_b = TIMER_OUTPUT_MODE_DEFAULT,
        .prescaler = TIMER_PRESCALER_8,
        .oca_mode = TIMER_OCA_MODE_TOP,
        // .interrupts = BIT(TIMER_INTERRUPT_OCM_A)|BIT(TIMER_INTERRUPT_OCM_B),
        .ocra = 79,
        .ocrb = 39
    };

    if (timer_init(TIMER_0, &timer_cfg) < 0)
    {
        serial_println(SERIAL_BUS_0, "INIT TIMER FAILED");
    }
    else if (timer_start(TIMER_0) < 0)
    {
        serial_println(SERIAL_BUS_0, "START TIMER FAILED");
    }

    icp_init(ICP1, ICP_PRESCALER_1);

    // Init tasks
    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);
    os_timer_start(TIMER_INTERVAL);

    os_watchdog_enable(OS_WATCHDOG_2S);

    serial_println(SERIAL_BUS_0, "READY");
}

// Main task
int task_main(void* data)
{
    UNUSED(data);

    os_watchdog_trigger();

    int ret = 0;

    float dutyCycle = 0;
    ret = icp_get_duty_cycle(ICP1, &dutyCycle);
    switch(ret)
    {
        case ICP_OK:
            snprintf(buffer, BUFFER_SZ, "duty cycle = %.03f", dutyCycle);
            serial_println(SERIAL_BUS_0, buffer);
            break;

        case -ICP_ERROR_OVERFLOW:
            serial_println(SERIAL_BUS_0, "No input");
            break;

        default:
            snprintf(buffer, BUFFER_SZ, "icp_get_duty_cycle : err %d", ret);
            serial_println(SERIAL_BUS_0, buffer);
            break;
    }

    uint16_t freq = 0;
    ret = icp_get_frequency(ICP1, &freq);
    switch(ret)
    {
        case ICP_OK:
            snprintf(buffer, BUFFER_SZ, "frequency = %uHz", freq);
            serial_println(SERIAL_BUS_0, buffer);
            break;

        case -ICP_ERROR_OVERFLOW:
            serial_println(SERIAL_BUS_0, "No input");
            break;

        default:
            snprintf(buffer, BUFFER_SZ, "icp_get_frequency : err %d", ret);
            serial_println(SERIAL_BUS_0, buffer);
            break;
    }

    return 0;
}
