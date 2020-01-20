#ifndef SRC_HC595_API_PRV_H_
#define SRC_HC595_API_PRV_H_

#include "types.h"
#include "port.h"
#include "hc595_cfg.h"

extern PinDef pinSerial, pinLatch, pinClock;

#if HC595_PIN_CLEAR == ON
extern PinDef pinClear;
#endif

void HC595_Delay();
void HC595_RisingEdge(PinDef pin);
void HC595_FallingEdge(PinDef pin);

#endif /* SRC_HC595_API_PRV_H_ */
