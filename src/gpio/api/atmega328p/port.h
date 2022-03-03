#ifndef PORT_LL_API_H__
#define PORT_LL_API_H__

#include "types.h"

// TODO name clash with avr/io.h

typedef enum {
    EXTINT0, // PD2
    EXTINT1, // PD3
    NUMBER_OF_EXTINTS
} extint_t;

typedef enum {
    PCINTB, // PB0..7 => PCINT0..7
    PCINTC, // PC0..6 => PCINT8..14
    PCINTD, // PD0..7 => PCINT16..23
    NUMBER_OF_PCINTS
} pcint_t;

typedef enum {
    PORT_B,
    PORT_C,
    PORT_D,
    NUMBER_OF_PORTS
} port_t;

#define PORTS __MEM(0x23, mem_port_t*)
#define PCMSK __MEM(0x6B, uint8_t*)

#endif // PORT_LL_API_H__
