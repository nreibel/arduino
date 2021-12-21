#include "stdio.h"
#include "os.h"
#include "app.h"
#include "timer.h"
#include "serial.h"

// App entry point
void app_init()
{
    serial_bus_init(SERIAL_BUS_0, 19200);

    timer_config_t timer_cfg = {
        .mode = TIMER_MODE_FAST_PWM,
        .output_mode = TIMER_OUTPUT_MODE_DEFAULT,
        .prescaler = TIMER_PRESCALER_8,
        .oca_mode = TIMER_OCA_MODE_TOP,
        .ocra = 79,
        .ocrb = 19
    };

    if (timer_init(TIMER_0, &timer_cfg) < 0)
    {
        serial_println(SERIAL_BUS_0, "INIT TIMER FAILED");
    }

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

    static bool is_running = FALSE;

    os_watchdog_trigger();

    if (is_running) timer_stop(TIMER_0);
    else timer_start(TIMER_0);
    is_running = !is_running;

    return 0;
}
