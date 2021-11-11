#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "serial_tp.h"
#include "i2c.h"
#include "tc74.h"
#include "pca954x.h"

// Allocate string buffer
char buffer[64];

// ALlocate TC74 object
static struct tc74_prv_s tc74_data;
static tc74_t tc74 = &tc74_data;

static struct pca954x_prv_s pca9544_data;
static pca954x_t pca9544 = &pca9544_data;

// App entry point
void app_init()
{
    // Init buses
    serial_bus_init(SERIAL_BUS_0, 19200);
    i2c_bus_init_master(I2C_BUS_0, TRUE);

    // Init I2C mux
    pca954x_init(pca9544, I2C_BUS_0, 0x70);
    pca954x_select(pca9544, 0);

    // Init TC74
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
