#ifndef OS_API_TYPES_H_
#define OS_API_TYPES_H_

#include <stdint.h>

typedef enum {
    Status_Not_OK = 0,
    Status_Invalid_Parameter,
    Status_Pending,
    Status_OK
} Std_ReturnType;

typedef enum {
    FALSE = 0,
    TRUE = 1
} bool;

typedef void (*Interrupt)(volatile void*);
typedef Std_ReturnType (*Callback)(void*);

typedef union {
    uint16_t value;
    uint8_t  byte[2];
} word;

typedef union {
    uint32_t value;
    uint16_t word[2];
    uint8_t  byte[4];
} dword;

typedef void* buffer_t;

#define UNUSED(x) (void)(x)

#define NOT(b) ((b) == FALSE ? TRUE : FALSE)

#define ON  1
#define OFF 0

// Debug
#define HALT while(1)

#define TYPECAST(var, type) ((type) (var))
#define PTR(addr, type)     TYPECAST(addr, type*)
#define UINT8_PTR(var)      PTR(var, uint8_t)
#define UINT16_PTR(var)     PTR(var, uint16_t)
#define UINT32_PTR(var)     PTR(var, uint32_t)
#define VOID_PTR(var)       PTR(var, void)
#define NULL_PTR            PTR(0, void)

#define READ_PTR(addr, type) (*PTR(addr, type))
#define READ_PU8(addr)       READ_PTR(addr, uint8_t)
#define READ_PU16(addr)      READ_PTR(addr, uint16_t)
#define READ_PU32(addr)      READ_PTR(addr, uint32_t)

#define WRITE_PTR(addr, val, type) *PTR(addr, type) = ((type)(val))
#define WRITE_PU8(addr, val)       WRITE_PTR(addr, val, uint8_t)
#define WRITE_PU16(addr, val)      WRITE_PTR(addr, val, uint16_t)
#define WRITE_PU32(addr, val)      WRITE_PTR(addr, val, uint32_t)

#endif /* OS_API_TYPES_H_ */
