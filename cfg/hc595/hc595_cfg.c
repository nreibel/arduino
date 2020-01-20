#include "hc595.h"
#include "hc595_prv.h"
#include "port.h"

PinDef pinSerial = {Port_B, 0};
PinDef pinLatch  = {Port_B, 1};
PinDef pinClock  = {Port_B, 2};

#if HC595_PIN_CLEAR == ON
PinDef pinClear  = {Port_B, 3};
#endif
