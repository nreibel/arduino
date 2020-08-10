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

int HC165_Read(void* buffer, int len)
{
    State pinState;

    Port_SetPinState(HC165_Pin_Latch, High);

    while(len-- > 0)
    {
        byte *data = TYPECAST(buffer++, byte*);
        for (int j = 7 ; j >= 0 ; j--)
        {
            Port_GetPinState(HC165_Pin_Serial, &pinState);
            if (pinState == High) SET_BIT(*data, j);
            Port_RisingEdge(HC165_Pin_Clock);
        }
    }

    Port_SetPinState(HC165_Pin_Latch, Low);
    return len;
}

uint8_t HC165_ReadByte()
{
    uint8_t data = 0;
    HC165_Read(&data, 1);
    return data;
}

uint16_t HC165_ReadWord()
{
    uint16_t data = 0;
    HC165_Read(&data, 2);
    return data;
}

uint32_t HC165_ReadDWord()
{
    uint32_t data = 0;
    HC165_Read(&data, 4);
    return data;
}
