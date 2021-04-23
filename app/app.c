#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"

// App entry point
void app_init()
{
    // Init buses
    serial_bus_init(SERIAL_BUS_0, 19200);

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

    return Status_OK;
}
