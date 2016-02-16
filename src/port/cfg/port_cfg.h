#ifndef SRC_PORT_CFG_PORT_CFG_H_
#define SRC_PORT_CFG_PORT_CFG_H_

#include "types.h"

typedef enum {
	Port_B = 0,
	Port_C,
	Port_D,
	NbrOfPorts
} Port;

extern uint8_t Port_Cfg_BaseAddr[NbrOfPorts];

#endif /* SRC_PORT_CFG_PORT_CFG_H_ */
