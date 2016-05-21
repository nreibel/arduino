#include "keys.h"
#include "keys_prv.h"
#include "port.h"
#include "app_prv.h"

static PinState keyState[NUMBER_OF_KEYS];

void Keys_Init()
{
	int cpt;
	for (cpt = 0 ; cpt < NUMBER_OF_KEYS ; cpt++)
	{
		keyState[cpt] = Low;
		Port_SetPinDataDirection(Keys_PinMapping[cpt], Input);
	}
}

void Keys_CyclicTask()
{
	int cpt;
	for (cpt = 0 ; cpt < NUMBER_OF_KEYS ; cpt++)
	{
		Port_GetPinState(Keys_PinMapping[cpt], &keyState[cpt]);
	}
}

boolean Keys_IsKeyPressed()
{
	int cpt;
	for (cpt = 0 ; cpt < NUMBER_OF_KEYS ; cpt++)
	{
		if (keyState[cpt] != Low)
		{
			return TRUE;
		}
	}

	return FALSE;
}

Std_ReturnType Keys_GetKeyPressed(Key *key)
{
	Std_ReturnType retval = Status_Not_OK;

	int cpt;
	for (cpt = 0 ; cpt < NUMBER_OF_KEYS ; cpt++)
	{
		if (keyState[cpt] != Low)
		{
			*key = (Key) cpt;
			retval = Status_OK;
		}
	}

	return retval;
}
