#include "hc165.h"
#include "hc165_prv.h"
#include "bits.h"
#include "gpio.h"

void HC165_Init()
{
    GPIO_SetDataDirection(HC165_Pin_Serial, GPIO_Input);
    GPIO_SetDataDirection(HC165_Pin_Clock, GPIO_Output);
    GPIO_SetDataDirection(HC165_Pin_Latch, GPIO_Output);

    GPIO_Set(HC165_Pin_Clock, GPIO_Low);
    GPIO_Set(HC165_Pin_Latch, GPIO_Low);
}

int HC165_Read(void* buffer, int len)
{
    GPIO_State pin = GPIO_Low;

    GPIO_Set(HC165_Pin_Latch, GPIO_High);

    for (int i = 0 ; i < len ; i++)
    {
        byte *data = TYPECAST(buffer++, byte*);
        for (int j = 7 ; j >= 0 ; j--)
        {
            GPIO_Get(HC165_Pin_Serial, &pin);

            if (pin == GPIO_High) SET_BIT(*data, j);

            GPIO_RisingEdge(HC165_Pin_Clock);
        }
    }

    GPIO_Set(HC165_Pin_Latch, GPIO_Low);

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
