#include "hc595.h"
#include "hc595_prv.h"
#include "hc595_cfg.h"
#include "port.h"

#include <util/delay.h>

void HC595_Init()
{
    Port_SetPinDataDirection(pinSerial, Output);
    Port_SetPinDataDirection(pinClock,  Output);
    Port_SetPinDataDirection(pinLatch,  Output);

    Port_SetPinState(pinSerial, Low);
    Port_SetPinState(pinClock,  Low);
    Port_SetPinState(pinLatch,  Low);

#if HC595_PIN_CLEAR == ON
    Port_SetPinDataDirection(pinClear, Output);
    Port_SetPinState(pinClear, High);
#endif
}

void HC595_Delay()
{
    _delay_us(1);
}

void HC595_RisingEdge(GPIO pin)
{
    Port_SetPinState(pin, High);
    HC595_Delay();
    Port_SetPinState(pin, Low);
}

void HC595_FallingEdge(GPIO pin)
{
    Port_SetPinState(pin, Low);
    HC595_Delay();
    Port_SetPinState(pin, High);
}

void HC595_ShiftBit(uint8_t bit)
{
    Port_SetPinState(pinSerial, bit ? High : Low);
    HC595_RisingEdge(pinClock);
}

void HC595_Latch()
{
    HC595_RisingEdge(pinLatch);
}

void HC595_ShiftByte(uint8_t val)
{
    for (int i = 0 ; i < 8 ; i++)
    {
        HC595_ShiftBit(val >> 7);
        val = val << 1;
    }
}

#if HC595_DAISY_CHAIN_COUNT > 1
void HC595_ShiftWord(uint16_t val)
{
    for (int i = 0 ; i < 16 ; i++)
    {
        HC595_ShiftBit(val >> 15);
        val = val << 1;
    }
}
#endif

#if HC595_DAISY_CHAIN_COUNT > 3
void HC595_ShiftDWord(uint32_t val)
{
    for (int i = 0 ; i < 32 ; i++)
    {
        HC595_ShiftBit(val >> 31);
        val = val << 1;
    }
}
#endif

#if HC595_PIN_CLEAR == ON
void HC595_Clear()
{
    HC595_FallingEdge(pinClear);
}
#endif
