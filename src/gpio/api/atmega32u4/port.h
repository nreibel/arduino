#ifndef PORT_LL_API_H__
#define PORT_LL_API_H__

#include "types.h"
#include "gpio_cfg.h"

// TODO name clash with avr/io.h

#if GPIO_EXTINT
typedef enum {
    EXTINT0, // PD0
    EXTINT1, // PD1
    EXTINT2, // PD2
    EXTINT3, // PD3
    EXTINT6, // PE6
    NUMBER_OF_EXTINTS
} extint_t;
#endif // GPIO_EXTINT

#if GPIO_PCINT
typedef enum {
    PCINTB, // PB0..7 => PCINT0..7
    NUMBER_OF_PCINTS
} pcint_t;
#endif // GPIO_PCINT

typedef enum {
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E,
    PORT_F,
    NUMBER_OF_PORTS
} port_t;

#define PORTS __MEM(0x23, mem_port_t*)
#define PCMSK __MEM(0x6B, uint8_t*)

#endif // PORT_LL_API_H__
