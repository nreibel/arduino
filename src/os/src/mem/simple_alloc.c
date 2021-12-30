#include "os_cfg.h"

#if OS_MALLOC == OS_MALLOC_SIMPLE

#include "os_mem.h"
#include "string.h"

/*
 * Private functions prototypes
 */

static void* my_malloc(unsigned int sz);
static unsigned int my_get_total();
static unsigned int my_get_free();
static unsigned int my_get_used();

/*
 * Private data
 */

static struct malloc_drv_s drv = {
    .malloc = my_malloc,
    .calloc = NULL_PTR,
    .free = NULL_PTR,
    .get_free = my_get_free,
    .get_used = my_get_used,
    .get_total = my_get_total
};

static uint8_t heap[OS_HEAP_SIZE];
static unsigned int idx = 0;

/*
 * Exported data
 */

const malloc_drv_t malloc_drv = &drv;

/*
 * Private functions
 */

static void* my_malloc(unsigned int sz)
{
    if (idx + sz > OS_HEAP_SIZE)
        return NULL_PTR;

    void *ptr = &heap[idx];
    idx += sz;
    return ptr;
}

static unsigned int my_get_used()
{
    return idx;
}

static unsigned int my_get_free()
{
    return OS_HEAP_SIZE-idx;
}

static unsigned int my_get_total()
{
    return OS_HEAP_SIZE;
}

#endif // OS_MALLOC == OS_MALLOC_SIMPLE
