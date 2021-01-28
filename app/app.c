#include "os.h"
#include "app.h"
#include "serial.h"

// App entry point
void App_Init()
{
    Serial_Init();
    Serial_Println("READY");

    Os_SetupTask(Timer_MainTask, 500, Task_MainCyclic, NULL_PTR);
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
