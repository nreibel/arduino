#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "i2c.h"
#include "pca954x.h"
#include "tc74.h"
#include "max31790.h"

// Allocate string buffer
#define BUFFER_SZ 64
char buffer[BUFFER_SZ];

static struct pca954x_prv_s pca954x_prv_s;
static pca954x_t pca9544 = &pca954x_prv_s;

static struct tc74_prv_s tc74_prv[4];
static tc74_t tc74[4] = {
    &tc74_prv[0],
    &tc74_prv[1],
    &tc74_prv[2],
    &tc74_prv[3],
};

static struct max31790_prv_s max31790_data;
max31790_t max31790 = &max31790_data;

// App entry point
void app_init()
{
    // Init buses
    i2c_atmega328p_init(FALSE);
    i2c_bus_t i2c_avr = i2c_get_bus(0);

    pca9544_init(pca9544, i2c_avr, 0x70);
    i2c_bus_t mux_bus_0 = i2c_get_bus(1);
    i2c_bus_t mux_bus_1 = i2c_get_bus(2);
    i2c_bus_t mux_bus_3 = i2c_get_bus(4);

    serial_bus_init(SERIAL_BUS_0, 19200);


    tc74_init(tc74[0], mux_bus_0, TC74A4);
    tc74_init(tc74[1], mux_bus_0, TC74A7);
    tc74_init(tc74[2], mux_bus_1, TC74A6);
    tc74_init(tc74[3], mux_bus_3, TC74A2);

    // Init MAX31790 fan controller
    int ret = max31790_init(max31790, i2c_avr, 0x20);
    if (ret < 0)
    {
        snprintf(buffer, BUFFER_SZ, "MAX31790 ERROR %d : %s", ret, i2c_get_error_string(ret));
        serial_println(SERIAL_BUS_0, buffer);
    }

    // Init tasks
    os_task_setup(TASK_MAIN, 1000, task_main, NULL_PTR);
    os_timer_start(TIMER_INTERVAL);

    os_watchdog_enable(OS_WATCHDOG_2S);

    // Ready!
    serial_println(SERIAL_BUS_0, "READY");
}

// Main task
int task_main(void* data)
{
    UNUSED(data);

    time_t time = os_timer_value(TIMER_INTERVAL);
    sprintf(buffer, "%ums", time);
    serial_println(SERIAL_BUS_0, buffer);

    for (int i = 0 ; i < 4 ; i++)
    {
        int temperature = 0;
        int retval = tc74_read_temperature(tc74[i], &temperature);
        if (retval < 0) snprintf(buffer, BUFFER_SZ, "I2C Error %d : %s", retval, i2c_get_error_string(retval));
        else snprintf(buffer, BUFFER_SZ, "t%d=%d", i, temperature);
        serial_println(SERIAL_BUS_0, buffer);
    }

    os_watchdog_trigger();

    return 0;
}
