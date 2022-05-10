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

#define NOT(b) (b ? false : true)

#define ON  1
#define OFF 0

#define TYPECAST(var, type) ((type) (var))
#define PTR(addr, type)     TYPECAST(addr, type*)
#define ADDR(var)           TYPECAST(&var, unsigned int)
#define UINT8_PTR(var)      PTR(var, uint8_t)
#define UINT16_PTR(var)     PTR(var, uint16_t)
#define UINT32_PTR(var)     PTR(var, uint32_t)
#define VOID_PTR(var)       PTR(var, void)
#define NULL_PTR            PTR(0, void)

#define READ_PTR(addr, type) (*PTR(addr, type))
#define READ_PU8(addr)       READ_PTR(addr, uint8_t)
#define READ_PU16(addr)      READ_PTR(addr, uint16_t)
#define READ_PU32(addr)      READ_PTR(addr, uint32_t)

#define WRITE_PTR(addr, val, type) *PTR(addr, type) = TYPECAST(val, type)
#define WRITE_PU8(addr, val)       WRITE_PTR(addr, val, uint8_t)
#define WRITE_PU16(addr, val)      WRITE_PTR(addr, val, uint16_t)
#define WRITE_PU32(addr, val)      WRITE_PTR(addr, val, uint32_t)

#endif /* OS_API_TYPES_H_ */
