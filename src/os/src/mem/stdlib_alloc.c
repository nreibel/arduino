#include "os_cfg.h"

#if OS_MALLOC == OS_MALLOC_STDLIB

#include "os_mem.h"
#include "string.h"
#include "stdlib.h"

/*
 * Private functions prototypes
 */

static void* my_malloc(unsigned int sz);
static void* my_calloc(unsigned int sz);
static void my_free(void *ptr);
static unsigned int my_get_free();
static unsigned int my_get_total();
static unsigned int my_get_used();

/*
 * Private data
 */

static struct malloc_drv_s drv = {
    .malloc = my_malloc,
    .calloc = my_calloc,
    .free = my_free,
    .get_free = my_get_free,
    .get_used = my_get_used,
    .get_total = my_get_total
};

/*
 * Exported data
 */

const malloc_drv_t malloc_drv = &drv;

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
    extern int __heap_start, *__brkval;
    return (unsigned int) __brkval - (unsigned int) &__heap_start;
}

static unsigned int my_get_free()
{
    extern int __heap_start, *__brkval;
    uint8_t v, *stack_end = &v;
    int *heap_end = __brkval == 0 ? &__heap_start : __brkval;
    return (unsigned int) stack_end - (unsigned int) heap_end;
}

static unsigned int my_get_total()
{
    extern int __heap_start;
    uint8_t v, *stack_end = &v;
    return (unsigned int) stack_end - (unsigned int) &__heap_start;
}

#endif // OS_MALLOC == OS_MALLOC_GCC
