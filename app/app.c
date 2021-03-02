#include "stdio.h"
#include "os.h"
#include "app.h"
#include "serial.h"
#include "gpio.h"
#include "max31855.h"


char buffer[64];
max31855_t max31855;

// App entry point
void App_Init()
{
    spi_init();

    max31855_device_init(&max31855, GPIO_D9);

    Serial_Init();
    Serial_Println("READY");

    Os_SetupTask(Timer_MainTask, 500, Task_MainCyclic, NULL_PTR);
}

// Main task
Std_ReturnType Task_MainCyclic(void* data)
{
    UNUSED(data);

    double temperature_int = 0.0;
    max31855_get_internal_temperature(&max31855, &temperature_int);
    sprintf(buffer, "Internal temperature is %f", temperature_int);
    Serial_Println(buffer);

    double temperature_ext = 0.0;
    if ( max31855_get_temperature(&max31855, &temperature_ext) )
    {
        sprintf(buffer, "Temperature is %f", temperature_ext);
        Serial_Println(buffer);
    }
    else
    {
        Serial_Println("Temperature probe error");
    }


    return Status_OK;
}
