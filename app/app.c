#include "os.h"
#include "app.h"
#include "port.h"
#include "serial.h"
#include "bits.h"
#include "gpio_uno.h"

PinDef Pin_LED = GPIO_13;

// App entry point
void App_Init()
{
    Port_SetPinDataDirection(Pin_LED, Output);

    // Init serial debug communication
    Serial_Init();

    // Set up tasks
    Os_SetupTask(Timer_MainTask, 1000, &Task_MainCyclic, NULL_PTR);
}

// Main task
Std_ReturnType Task_MainCyclic(void* data)
{
    UNUSED(data);

    static PinState state = High;

    if (state == High)
    {
        Port_SetPinState(Pin_LED, High);
        Serial_PrintLine("Tick");
        state = Low;
    }
    else
    {
        Port_SetPinState(Pin_LED, Low);
        Serial_PrintLine("Tock");
        state = High;
    }

    return Status_OK;
}
