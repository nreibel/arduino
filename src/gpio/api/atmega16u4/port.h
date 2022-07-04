#ifndef PORT_LL_API_H__
#define PORT_LL_API_H__

#include "types.h"

// TODO name clash with avr/io.h

typedef enum {
    EXTINT0, // PD0
    EXTINT1, // PD1
    EXTINT2, // PD2
    EXTINT3, // PD3
    EXTINT6, // PE6
    NUMBER_OF_EXTINTS
} extint_t;

typedef enum {
    PCINTB, // PB0..7 => PCINT0..7
    NUMBER_OF_PCINTS
} pcint_t;

typedef enum {
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E,
    PORT_F,
    NUMBER_OF_PORTS
} port_t;

#define PORTS TYPECAST(0x23, mem_port_t)
#define PCMSK TYPECAST(0x6B, volatile uint8_t *)

#endif // PORT_LL_API_H__
