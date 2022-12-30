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

typedef struct {
    uint8_t pin;
    uint8_t ddr;
    uint8_t port;
} volatile * mem_port_t;

#define PORTS TYPECAST(0x23, mem_port_t)
#define PCMSK TYPECAST(0x6B, volatile uint8_t *)
#define EICR  TYPECAST(0x69, volatile uint8_t *)

#endif // PORT_LL_API_H__
