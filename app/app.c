#include "stdio.h"
#include "os.h"
#include "os_mem.h"
#include "app.h"
#include "timer.h"
#include "icp.h"
#include "serial.h"

/*
 * Timer demo :
 * - Timer 0 : Output 200Hz PWM at 50% duty cycle on OC0B (D5)
 * - Timer 1 : Input Capture mode on ICP1 (D8)
 * - Timer 2 : OS Timer (1ms counter)
 *
 * Link OC0B (D5) and ICP1 (D8) together to read PWM
 * Result is displayed on serial output
 * Shoud read 200Hz and 127 (50%) duty cycle
 */

const __flash char STR_READY[] = "Ready!\r\n";
const __flash char STR_ERROR[] = "Error %d\r\n";
const __flash char STR_OVERFLOW[] = "Overflow\r\n";
const __flash char STR_NO_INPUT[] = "No input\r\n";
const __flash char STR_FREQUENCY[] = "Frequency = %uHz\r\n";
const __flash char STR_DUTY_CYCLE[] = "Duty cycle = %u\r\n";
const __flash char STR_ERROR_ICP_INIT[] = "INIT ICP FAILED\r\n";
const __flash char STR_ERROR_TIMER_INIT[] = "INIT TIMER FAILED\r\n";
const __flash char STR_ERROR_TIMER_START[] = "START TIMER FAILED\r\n";

void serial_rx_cbk(serial_bus_t bus, const char *buffer, int length)
{
    UNUSED(bus);
    UNUSED(length);
    printf(buffer);
}

int os_putc(char chr, FILE *stream)
{
    UNUSED(stream);
    if (serial_write_byte(SERIAL_BUS_0, chr) != 1) return EOF;
    else return chr;
}

// App entry point
void app_init()
{
    serial_bus_init(SERIAL_BUS_0, 19200);
    serial_set_rx_callback(SERIAL_BUS_0, serial_rx_cbk);

    timer_config_t timer_cfg = {
        .mode = TIMER_MODE_FAST_PWM,
        .output_mode_b = TIMER_OUTPUT_MODE_DEFAULT,
        .oca_mode = TIMER_OCA_MODE_TOP,
        .prescaler = TIMER_PRESCALER_1024,
        .ocra = 77,
        .ocrb = 38,
    };

    if (timer_init(TIMER_0, &timer_cfg) < 0)
    {
        printf_P(STR_ERROR_TIMER_INIT);
        HALT();
    }

    if (timer_start(TIMER_0) < 0)
    {
        printf_P(STR_ERROR_TIMER_START);
        HALT();
    }

    /* Use non-blocking mode */
    if (icp_init(ICP1, ICP_PRESCALER_256, TRUE) < 0)
    {
        printf_P(STR_ERROR_ICP_INIT);
        HALT();
    }

    // Init tasks
    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);
    os_timer_start(TIMER_INTERVAL);

    os_watchdog_enable(OS_WATCHDOG_2S);

#if OS_MALLOC
    char *buf = os_malloc(64);
    UNUSED(buf);

    printf("Heap usage : %u used, %u free, %u total\r\n", os_get_used_heap(), os_get_free_heap(), os_get_total_heap());
#endif

    printf_P(STR_READY);
}

// Main task
int task_main(void* data)
{
    UNUSED(data);

    int ret = 0;
    uint8_t duty_cycle = 0;
    uint16_t freq = 0;

    os_watchdog_trigger();

    ret = icp_get_duty_cycle(ICP1, &duty_cycle);
    switch(ret)
    {
        case ICP_OK:
            printf_P(STR_DUTY_CYCLE, duty_cycle);
            break;

        case -ICP_ERROR_NO_DATA:
            printf_P(STR_NO_INPUT);
            break;

        case -ICP_ERROR_OVERFLOW:
            printf_P(STR_OVERFLOW);
            break;

        default:
            printf_P(STR_ERROR, ret);
            break;
    }

    ret = icp_get_frequency(ICP1, &freq);
    switch(ret)
    {
        case ICP_OK:
            printf_P(STR_FREQUENCY, freq);
            break;

        case -ICP_ERROR_NO_DATA:
            printf_P(STR_NO_INPUT);
            break;

        case -ICP_ERROR_OVERFLOW:
            printf_P(STR_OVERFLOW);
            break;

        default:
            printf_P(STR_ERROR, ret);
            break;
    }

    return 0;
}
