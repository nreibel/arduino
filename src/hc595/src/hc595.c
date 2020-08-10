#include "hc595.h"
#include "hc595_prv.h"
#include "bits.h"
#include "port.h"

void HC595_Init()
{
    Port_SetPinDataDirection(HC595_Pin_Serial, Output);
    Port_SetPinDataDirection(HC595_Pin_Clock, Output);
    Port_SetPinDataDirection(HC595_Pin_Latch, Output);

    Port_SetPinState(HC595_Pin_Serial, Low);
    Port_SetPinState(HC595_Pin_Clock, Low);
    Port_SetPinState(HC595_Pin_Latch, Low);
}

void HC595_Write(void* data, int len)
{
    for (int i = len-1 ; i >= 0 ; i--)
    {
        byte val = READ_PU8(data+i);
        for (int i = 0 ; i < 8 ; i++)
        {
            Port_SetPinState(HC595_Pin_Serial, GET_BIT(val, 7) ? High : Low);
            Port_RisingEdge(HC595_Pin_Clock);
            val = val << 1;
        }
    }

    Port_RisingEdge(HC595_Pin_Latch);
}

void HC595_WriteByte(uint8_t val)
{
    HC595_Write(&val, 1);
}

void HC595_WriteWord(uint16_t val)
{
    HC595_Write(&val, 2);
}

void HC595_WriteDWord(uint32_t val)
{
    HC595_Write(&val, 4);
}
