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

/*
 * Private data
 */

static struct malloc_drv_s drv = {
    .malloc = my_malloc,
    .calloc = my_calloc,
    .free = my_free,
    .get_free = NULL_PTR,
    .get_used = NULL_PTR,
    .get_total = NULL_PTR
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

#endif // OS_MALLOC == OS_MALLOC_GCC
