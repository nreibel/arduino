#include "hc165.h"
#include "hc165_prv.h"
#include "bits.h"
#include "port.h"

void HC165_Init()
{
    Port_SetPinDataDirection(HC165_Pin_Serial, Input);
    Port_SetPinDataDirection(HC165_Pin_Clock, Output);
    Port_SetPinDataDirection(HC165_Pin_Latch, Output);

    Port_SetPinState(HC165_Pin_Clock, Low);
    Port_SetPinState(HC165_Pin_Latch, Low);
}

uint8_t HC165_ReadByte()
{
    byte data = 0;
    State pinState;

    Port_SetPinState(HC165_Pin_Latch, High);

    for (int i = 7 ; i >= 0 ; i--)
    {
        Port_GetPinState(HC165_Pin_Serial, &pinState);
        if (pinState == High) SET_BIT(data, i);
        Port_RisingEdge(HC165_Pin_Clock);
    }

    Port_SetPinState(HC165_Pin_Latch, Low);
    return data;
}
