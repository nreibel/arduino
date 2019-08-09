#include "keys.h"
#include "keys_cfg.h"
#include "port.h"

/*
 * Assign a pin to each key.
 * Must be defined in the same order as enum Key
 */
PinDef Keys_PinMapping[NUMBER_OF_KEYS] = {
    {Port_D, Pin_7}, // ScrollUp
    {Port_D, Pin_6}, // ScrollDown
};
