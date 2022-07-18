#ifndef OS_API_TYPES_H_
#define OS_API_TYPES_H_

#include <stdint.h>
#include <stdbool.h>

typedef union {
    uint16_t value;
    uint8_t  bytes[2];
} word;

typedef union {
    uint32_t value;
    uint16_t words[2];
    uint8_t  bytes[4];
} dword;

// String stored in flash memory
typedef const __flash char flstr_t[];

#define UNUSED(x) (void)(x)

#define ON  1
#define OFF 0

#define TYPECAST(var, type) ((type) (var))
#define PTR(addr, type)     TYPECAST(addr, type*)
#define ADDR(var)           TYPECAST(&var, unsigned int)
#define PU8(var)            PTR(var, uint8_t)
#define PU16(var)           PTR(var, uint16_t)
#define PU32(var)           PTR(var, uint32_t)
#define NULL_PTR            PTR(0, void)

#endif /* OS_API_TYPES_H_ */
