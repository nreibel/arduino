#include "hc595.h"
#include "hc595_prv.h"
#include "bits.h"
#include "gpio.h"

void HC595_Init()
{
    GPIO_SetDataDirection(HC595_Pin_Serial, GPIO_Output);
    GPIO_SetDataDirection(HC595_Pin_Clock, GPIO_Output);
    GPIO_SetDataDirection(HC595_Pin_Latch, GPIO_Output);

    GPIO_SetState(HC595_Pin_Serial, FALSE);
    GPIO_SetState(HC595_Pin_Clock, FALSE);
    GPIO_SetState(HC595_Pin_Latch, FALSE);
}

void HC595_Write(buffer_t data, int len)
{
    for (int i = len ; i > 0 ; i--)
    {
        uint8_t val = READ_PU8(data+i-1);
        for (int j = 8 ; j > 0 ; j--)
        {
            GPIO_State st = IS_SET_BIT(val, j-1) ? GPIO_High : GPIO_Low;
            GPIO_SetState(HC595_Pin_Serial, st);
            GPIO_RisingEdge(HC595_Pin_Clock);
        }
    }

    GPIO_RisingEdge(HC595_Pin_Latch);
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
