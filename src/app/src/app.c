#include "os.h"
#include "app.h"
#include "app_prv.h"
#include "app_cfg.h"
#include "timer.h"
#include "port.h"
#include "serial.h"
#include "bits.h"
#include "eeprom.h"
#include "keys.h"


void App_Init()
{
	Port_SetPinDataDirection(Pin_LED, Output);

	// Init serial debug communication
	Serial_Init();

	// Set up tasks
	Timer_StartTask(Timer_MainTask, 1000, &Task_MainCyclic);
}

// Main task
void Task_MainCyclic(void)
{
    static PinState state = High;
    static char *message = "Tick\r\n";

    Port_SetPinState(Pin_LED, state);
    Serial_Print(message, 6);
    
    if (state == High)
    {
        state = Low;
        message = "Tock\r\n";
    }
    else
    {
        state = High;
        message = "Tick\r\n";
    }
}
