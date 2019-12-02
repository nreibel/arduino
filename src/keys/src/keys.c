#include "keys.h"
#include "keys_prv.h"
#include "port.h"
#include "bits.h"
#include "app_prv.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile PinState keyState[NUMBER_OF_KEYS];

void Keys_CyclicTask()
{
    for (int cpt = 0 ; cpt < NUMBER_OF_KEYS ; cpt++)
    {
        Port_GetPinState(Keys_PinMapping[cpt], &PTR(keyState, PinState)[cpt]);
    }
}

void Keys_Init()
{
    for (int cpt = 0 ; cpt < NUMBER_OF_KEYS ; cpt++)
    {
        keyState[cpt] = High;
        Port_SetPinDataDirection(Keys_PinMapping[cpt], Input);
        Port_SetPinState(Keys_PinMapping[cpt], High);
    }    
}

boolean Keys_IsKeyPressed()
{
    for (int cpt = 0 ; cpt < NUMBER_OF_KEYS ; cpt++)
    {
        if (keyState[cpt] != High)
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

Std_ReturnType Keys_GetKeyPressed(Key *key)
{
    Std_ReturnType retval = Status_Not_OK;
    
    for (int cpt = 0 ; cpt < NUMBER_OF_KEYS ; cpt++)
    {
        if (keyState[cpt] != High)
        {
            *key = (Key) cpt;
            keyState[cpt] = High;
            retval = Status_OK;
        }
    }
    
    return retval;
}
