#include "os.h"
#include "types.h"
#include "port_cfg.h"

uint8_t Port_Cfg_BaseAddr[NbrOfPorts] = {
    0x23, /* Port B */
    0x26, /* Port C */
    0x29, /* Port D */
};
