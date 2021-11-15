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

// App entry point
void app_init()
{
    // Init buses
    i2c_bus_init_master(I2C_BUS_0, FALSE);
    serial_bus_init(SERIAL_BUS_0, 19200);

    // Init tasks
    os_task_setup(TASK_MAIN, 100, Task_Main, NULL_PTR);
    os_timer_start(TIMER_INTERVAL);

    // Ready!
    serial_println(SERIAL_BUS_0, "READY");
}

// Main task
Std_ReturnType Task_Main(void* data)
{
    UNUSED(data);

    time_t time = os_timer_value(TIMER_INTERVAL);
    sprintf(buffer, "%lums", time);
    serial_println(SERIAL_BUS_0, buffer);

    return Status_OK;
}
