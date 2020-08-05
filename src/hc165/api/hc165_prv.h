#ifndef SRC_HC165_API_PRV_H_
#define SRC_HC165_API_PRV_H_

#include "types.h"
#include "port.h"
#include "hc165_cfg.h"

extern GPIO HC165_Pin_Serial;
extern GPIO HC165_Pin_Load;
extern GPIO HC165_Pin_Clock;
extern GPIO HC165_Pin_ClockEnable;

void HC165_Delay();

// TODO : move to PORT module
void HC165_RisingEdge(GPIO pin);
void HC165_FallingEdge(GPIO pin);

#endif /* SRC_HC165_API_PRV_H_ */
