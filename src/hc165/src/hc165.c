#include "hc165.h"
#include "hc165_prv.h"
#include "hc165_cfg.h"
#include "port.h"
#include "os_cfg.h"

#include <util/delay.h>

void HC165_Init()
{
    Port_SetPinDataDirection(HC165_Pin_Serial, Input);
    Port_SetPinDataDirection(HC165_Pin_Clock, Output);
    Port_SetPinDataDirection(HC165_Pin_Load, Output);
    Port_SetPinDataDirection(HC165_Pin_ClockEnable, Output);

    Port_SetPinState(HC165_Pin_Clock, Low);
    Port_SetPinState(HC165_Pin_Load, Low);
    Port_SetPinState(HC165_Pin_ClockEnable, Low);
}

void HC165_Delay()
{
    _delay_us(1);
}

void HC165_RisingEdge(GPIO pin)
{
    Port_SetPinState(pin, High);
    HC165_Delay();
    Port_SetPinState(pin, Low);
}

void HC165_FallingEdge(GPIO pin)
{
    Port_SetPinState(pin, Low);
    HC165_Delay();
    Port_SetPinState(pin, High);
}
