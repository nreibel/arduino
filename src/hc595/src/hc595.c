#include "hc595.h"
#include "hc595_prv.h"
#include "bits.h"
#include "gpio.h"

void HC595_Init()
{
    GPIO_SetDataDirection(HC595_Pin_Serial, GPIO_Output);
    GPIO_SetDataDirection(HC595_Pin_Clock, GPIO_Output);
    GPIO_SetDataDirection(HC595_Pin_Latch, GPIO_Output);

    GPIO_Set(HC595_Pin_Serial, FALSE);
    GPIO_Set(HC595_Pin_Clock, FALSE);
    GPIO_Set(HC595_Pin_Latch, FALSE);
}

void HC595_Write(void* data, int len)
{
    for (int i = len-1 ; i >= 0 ; i--)
    {
        byte val = READ_PU8(data+i);
        for (int i = 0 ; i < 8 ; i++)
        {
            GPIO_Set(HC595_Pin_Serial, GET_BIT(val, 7) ? GPIO_High : GPIO_Low);
            GPIO_RisingEdge(HC595_Pin_Clock);
            val = val << 1;
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
