#ifndef SRC_HC595_API_PRV_H_
#define SRC_HC595_API_PRV_H_

#include "types.h"
#include "port.h"
#include "hc595_cfg.h"

extern GPIO pinSerial, pinLatch, pinClock;

#if HC595_PIN_CLEAR == ON
extern GPIO pinClear;
#endif

void HC595_Delay();
void HC595_RisingEdge(GPIO pin);
void HC595_FallingEdge(GPIO pin);

#endif /* SRC_HC595_API_PRV_H_ */
