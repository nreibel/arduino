#include "types.h"
#include "stack.h"
#include <string.h>
#include <stdlib.h>

Stack* Stack_Create(uint16_t size)
{
    Stack *s = malloc(sizeof(Stack));
    s->buffer = calloc(size, sizeof(uint16_t));
    s->ptr    = s->buffer;
    s->size   = size;
    return s;
}

void Stack_Init(Stack *s, uint16_t value)
{
	uint16_t cpt;
	for (cpt = 0 ; cpt < s->size ; cpt++)
	{
		s->buffer[cpt] = value;
	}
}

void Stack_Push(Stack *s, uint16_t value)
{
    *(s->ptr++) = value;
    if (s->ptr >= s->buffer + s->size)
    {
        s->ptr = s->buffer;
    }
}
