#include "os.h"
#include "app.h"
#include "serial.h"

char buffer[64];

// App entry point
void App_Init()
{
    // Init serial
    Serial_Init();

    // Set up tasks
    Os_SetupTask(Timer_MainTask, 1000, &Task_MainCyclic, NULL_PTR);
}

// Main task
Std_ReturnType Task_MainCyclic(void* data)
{
    UNUSED(data);

    static bool state = TRUE;

    if (state) Serial_Println("Tick");
    else Serial_Println("Tock");

    state = !state;

    return Status_OK;
}
