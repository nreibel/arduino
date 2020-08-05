#include "hc595_cfg.h"
#include "port.h"

GPIO HC595_Pin_Serial = {Port_B, 0};
GPIO HC595_Pin_Latch  = {Port_B, 1};
GPIO HC595_Pin_Clock  = {Port_B, 2};

#if HC595_PIN_CLEAR == ON
GPIO HC595_Pin_Clear  = {Port_B, 3};
#endif
