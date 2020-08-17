#include "os.h"
#include "app.h"
#include "serial.h"
#include "bits.h"
#include "gpio.h"

GPIO Pin_LED = D13;

// App entry point
void App_Init()
{
    GPIO_SetDataDirection(Pin_LED, GPIO_Output);

    // Init serial debug communication
    Serial_Init();

    // Set up tasks
    Os_SetupTask(Timer_MainTask, 1000, &Task_MainCyclic, NULL_PTR);
}

// Main task
Std_ReturnType Task_MainCyclic(void* data)
{
    UNUSED(data);

    static bool state = TRUE;

    if (state)
    {
        GPIO_Set(Pin_LED, TRUE);
        Serial_PrintLine("Tick");
        state = FALSE;
    }
    else
    {
        GPIO_Set(Pin_LED, FALSE);
        Serial_PrintLine("Tock");
        state = TRUE;
    }

    return Status_OK;
}
