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

// Dynamically allocated
static tc74_t tc74 = NULL_PTR;

// Statically allocated of PCA9544
static struct pca954x_prv_s pca9544_data;
static pca954x_t pca9544 = &pca9544_data;

// App entry point
void app_init()
{
    // Init buses
    serial_bus_init(SERIAL_BUS_0, 19200);
    i2c_bus_init_master(I2C_BUS_0, FALSE);

    // Init I2C mux
    pca954x_init(pca9544, I2C_BUS_0, 0x70);

    // Init TC74
    tc74 = tc74_create(I2C_BUS_0, TC74A4);

    if (tc74 == NULL_PTR)
    {
        serial_println(SERIAL_BUS_0, "TC74 memory allocation failed");
        HALT;
    }

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
    sprintf(buffer, "Temperature is %d°C", temperature);
    serial_println(SERIAL_BUS_0, buffer);

    return Status_OK;
}
