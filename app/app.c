#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "i2c.h"

// App entry point
void app_init()
{
    // Init buses
    serial_bus_init(SERIAL_BUS_0, 19200);
    i2c_bus_init_master(I2C_BUS_0, TRUE);

    // Init tasks
    os_task_setup(TIMER_MAIN, 1000, Task_Main, NULL_PTR);

    // Ready!
    serial_println(SERIAL_BUS_0, "READY");
}

// Main task
Std_ReturnType Task_Main(void* data)
{
    UNUSED(data);

    serial_println(SERIAL_BUS_0, "Tick");
    serial_write_async(SERIAL_BUS_0, "Async\r\n", 7);

    return Status_OK;
}
