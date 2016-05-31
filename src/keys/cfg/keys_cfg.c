#include "keys.h"
#include "keys_cfg.h"
#include "port.h"

/*
 * Assign a pin to each key.
 * Must be defined in the same order as enum Key
 */
PinDef Keys_PinMapping[NUMBER_OF_KEYS] = {
		{Port_C, Pin_0}, // ScrollUp
		{Port_C, Pin_1}, // ScrollDown
		{Port_C, Pin_2}, // Preset1
		{Port_C, Pin_3}, // Preset2
		{Port_C, Pin_4}, // Preset3
		{Port_C, Pin_5}, // Preset4
};
