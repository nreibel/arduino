#ifndef SRC_PORT_CFG_PORT_CFG_H_
#define SRC_PORT_CFG_PORT_CFG_H_

#include "os.h"

typedef enum {
	Port_B = 0,
	Port_C,
	Port_D,
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
