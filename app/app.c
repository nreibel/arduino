#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "hc165.h"

// Allocate string buffer
char buffer[64];

static struct hc165_prv_s hc165_data;
static hc165_t hc165 = &hc165_data;

// App entry point
void app_init()
{
    // Init buses
    serial_bus_init(SERIAL_BUS_0, 19200);

    hc165_init(hc165);

    // Init tasks
    os_task_setup(TASK_MAIN, 1000, Task_Main, NULL_PTR);
    os_timer_start(TIMER_INTERVAL);

    // Ready!
    serial_println(SERIAL_BUS_0, "READY");
}

// Main task
Std_ReturnType Task_Main(void* data)
{
    UNUSED(data);

    time_t time = os_timer_get_value(TIMER_INTERVAL);
    sprintf(buffer, "t=%us", time/1000);
    serial_println(SERIAL_BUS_0, buffer);

    uint16_t input = hc165_read_word(hc165);
    sprintf(buffer, "inputs=%02x", input);
    serial_println(SERIAL_BUS_0, buffer);

    return Status_OK;
}
