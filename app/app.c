#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "i2c.h"
#include "max31790.h"

static max31790_t max31790;

// App entry point
void App_Init()
{
    // Init buses
    serial_bus_init(SERIAL_BUS_0, 19200);
    i2c_bus_init_master(I2C_BUS_0, FALSE);

    // Init MAX31790
    max31790_init(&max31790, I2C_BUS_0, 0x20);
    max31790_set_frequency(&max31790, MAX31790_FREQUENCY_25_KHZ);
    max31790_set_watchdog(&max31790, MAX31790_WATCHDOG_5_SECONDS);
    max31790_set_target_pwm(&max31790, MAX31790_FAN_0, 0);
    max31790_set_target_pwm(&max31790, MAX31790_FAN_1, 0);

    // Init tasks
    Os_SetupTask(Timer_MainTask, 500, Task_Main, NULL_PTR);

    // Ready!
    serial_println(SERIAL_BUS_0, "READY");
}

// Main task
Std_ReturnType Task_Main(void* data)
{
    UNUSED(data);

    max31790_set_target_pwm(&max31790, 0, 0x40);
    max31790_set_target_pwm(&max31790, 1, 0x80);

    return Status_OK;
}
