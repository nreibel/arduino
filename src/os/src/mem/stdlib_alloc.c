#include "os_mem.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>

/*
 * Private defines
 */

#define ADDR(var) TYPECAST(&(var), unsigned int)

/*
 * Private functions prototypes
 */

static void *       my_malloc(unsigned int sz);
static void *       my_calloc(unsigned int sz);
static void         my_free(void * ptr);
static unsigned int my_get_free (void);
static unsigned int my_get_total(void);
static unsigned int my_get_used (void);

/*
 * Exported data
 */

const struct malloc_drv_s stdlib_malloc = {
    .malloc    = my_malloc,
    .calloc    = my_calloc,
    .free      = my_free,
    .get_free  = my_get_free,
    .get_used  = my_get_used,
    .get_total = my_get_total
};

/*
 * Linker variables
 */

extern const int __heap_start;
extern const int *__brkval;

/*
 * Private functions
 */

static void* my_malloc(unsigned int sz)
{
    return malloc(sz);
}

static void* my_calloc(unsigned int sz)
{
    return calloc(sz, 1);
}

static void my_free(void * ptr)
{
    free(ptr);
}

static unsigned int my_get_used()
{
    return ADDR(*__brkval) - ADDR(__heap_start);
}

static unsigned int my_get_free()
{
    uint8_t v = 0, *stack_end = &v;
    unsigned int heap_end = __brkval == NULL_PTR ? ADDR(__heap_start) : ADDR(*__brkval);
    return (unsigned int) stack_end - heap_end;
}

static unsigned int my_get_total()
{
    uint8_t v = 0, *stack_end = &v;
    return (unsigned int) stack_end - ADDR(__heap_start);
}
