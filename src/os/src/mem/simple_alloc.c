#include "os_mem.h"
#include <string.h>

/*
 * Private functions prototypes
 */

static void *       my_malloc(unsigned int sz);
static unsigned int my_get_total(void);
static unsigned int my_get_free (void);
static unsigned int my_get_used (void);

/*
 * Private data
 */

static uint8_t heap[OS_HEAP_SIZE];
static unsigned int idx = 0;

/*
 * Exported data
 */

const struct malloc_drv_s simple_malloc = {
    .malloc    = my_malloc,
    .calloc    = NULL_PTR,
    .free      = NULL_PTR,
    .get_free  = my_get_free,
    .get_used  = my_get_used,
    .get_total = my_get_total
};

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
