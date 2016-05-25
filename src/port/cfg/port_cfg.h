#ifndef SRC_PORT_CFG_PORT_CFG_H_
#define SRC_PORT_CFG_PORT_CFG_H_

#include "os.h"

typedef enum {
#if ARCHITECTURE == ARCH_ATTINY84
	Port_A = 0,
	Port_B,
#elif ARCHITECTURE == ARCH_ATMEGA328
	Port_B = 0,
	Port_C,
	Port_D,
#endif
	NbrOfPorts
} Port;

typedef enum
{
	Pin_0 = 0,
	Pin_1,
	Pin_2,
	Pin_3,
	Pin_4,
	Pin_5,
	Pin_6,
	Pin_7,
	NbrOfPins
} Pin;

#endif /* SRC_PORT_CFG_PORT_CFG_H_ */
