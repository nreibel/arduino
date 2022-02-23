#ifndef PORT_LL_API_H__
#define PORT_LL_API_H__

#include "port.h"

#define PORTB TYPECAST(0x23, volatile const port_t)
#define PORTC TYPECAST(0x26, volatile const port_t)
#define PORTD TYPECAST(0x29, volatile const port_t)

#define REG_EIMSK (*TYPECAST(0x3D, volatile const eimsk_t))
#define REG_EICRA (*TYPECAST(0x69, volatile const eicr_t))

#endif // PORT_LL_API_H__
