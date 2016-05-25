#include "os.h"
#include "types.h"
#include "port_cfg.h"

uint8_t Port_Cfg_BaseAddr[NbrOfPorts] = {
#if ARCHITECTURE == ARCH_ATTINY84
		0x39, /* Port A */
		0x36, /* Port B */
#elif ARCHITECTURE == ARCH_ATMEGA328
		0x23, /* Port B */
		0x26, /* Port C */
		0x29, /* Port D */
#endif
};
