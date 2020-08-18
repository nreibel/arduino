#ifndef SRC_STACK_API_STACK_H_
#define SRC_STACK_API_STACK_H_

#include "types.h"

typedef struct {
    uint16_t* buffer; // Data
    uint16_t* ptr;    // Current element
    uint16_t  size;   // Max size
} Stack;

Stack* Stack_Create (uint16_t size);
void   Stack_Init   (Stack *s, uint16_t value);
void   Stack_Push   (Stack *s, uint16_t value);

#endif /* SRC_STACK_API_STACK_H_ */
