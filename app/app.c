#include "stdio.h"
#include "os.h"
#include "app.h"
#include "i2c.h"
#include "serial.h"
#include "tc74.h"
#include "hc165.h"
#include "hc595.h"

// Allocate string buffer
char buffer[64];

static hc165_t hc165 = NULL_PTR;
static hc595_t hc595 = NULL_PTR;
static tc74_t tc74 = NULL_PTR;

// App entry point
void app_init()
{
    // Init buses
    i2c_bus_init_master(I2C_BUS_0, FALSE);
    serial_bus_init(SERIAL_BUS_0, 19200);

    hc165 = hc165_create(A1, A3, A2);
    hc595 = hc595_create(D2, D4, D3);

    tc74 = tc74_create(I2C_BUS_0, TC74A7);

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
    int temperature = 0;

    tc74_read_temperature(tc74, &temperature);

    sprintf(buffer, "%us T=%u°C", time/1000, temperature);
    serial_println(SERIAL_BUS_0, buffer);

    return Status_OK;
}
