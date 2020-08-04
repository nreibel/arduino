#include "hc595_cfg.h"
#include "port.h"

GPIO pinSerial = {Port_B, 0};
GPIO pinLatch  = {Port_B, 1};
GPIO pinClock  = {Port_B, 2};

#if HC595_PIN_CLEAR == ON
GPIO pinClear  = {Port_B, 3};
#endif
