#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "hc165.h"
#include "hc595.h"

// Allocate string buffer
char buffer[64];

static struct hc165_prv_s hc165_data;
static hc165_t hc165 = &hc165_data;

static struct hc595_prv_s hc595_data;
static hc595_t hc595 = &hc595_data;

// App entry point
void app_init()
{
    // Init buses
    serial_bus_init(SERIAL_BUS_0, 19200);

    hc165_init(hc165, A1, A3, A2);
    hc595_init(hc595, D2, D4, D3);

    // Init tasks
    os_task_setup(TASK_MAIN, 200, Task_Main, NULL_PTR);
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
    sprintf(buffer, "inputs=%04x", input);
    serial_println(SERIAL_BUS_0, buffer);

    static uint16_t cpt = 1;
    hc595_write_word(hc595, cpt++);

    return Status_OK;
}
