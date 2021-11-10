#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "serial_tp.h"
#include "i2c.h"
#include "tc74.h"

// Allocate string buffer
char buffer[64];

// ALlocate TC74 object
static struct tc74_prv_s tc74_data;
static tc74_h tc74 = &tc74_data;

// App entry point
void app_init()
{
    // Init buses
    serial_bus_init(SERIAL_BUS_0, 19200);
    i2c_bus_init_master(I2C_BUS_0, TRUE);

    // Init TC74_REG_RTR
    tc74_init(tc74, I2C_BUS_0, TC74A4);

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

    int temperature = 0;

    tc74_read_temperature(tc74, &temperature);

    int len = sprintf(buffer, "Temperature is %d°C\r\n", temperature);
    serial_write_async(SERIAL_BUS_0, buffer, len);

    return Status_OK;
}
