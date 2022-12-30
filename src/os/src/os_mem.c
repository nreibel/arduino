#include "os.h"
#include "os_cfg.h"
#include "os_mem.h"
#include <string.h>

extern const struct malloc_drv_s simple_malloc;
extern const struct malloc_drv_s stdlib_malloc;

#if OS_MALLOC == OS_MALLOC_NONE
const malloc_drv_t malloc_drv = NULL_PTR;
#elif OS_MALLOC == OS_MALLOC_SIMPLE
const malloc_drv_t malloc_drv = &simple_malloc;
#elif OS_MALLOC == OS_MALLOC_STDLIB
const malloc_drv_t malloc_drv = &stdlib_malloc;
#else
#error Invalid value of OS_MALLOC
#endif

/*
 * Public functions
 */

#if OS_MALLOC != OS_MALLOC_NONE

void* os_calloc(unsigned int sz)
{
    void *ptr = NULL_PTR;

    if (malloc_drv->calloc != NULL_PTR)
    {
        // Driver implementation
        ptr = malloc_drv->calloc(sz);
    }
    else if (malloc_drv->malloc != NULL_PTR)
    {
        // Fallback to malloc + memset
        ptr = malloc_drv->malloc(sz);
        if (ptr != NULL_PTR) memset(ptr, 0, sz);
    }

    return ptr;
}

void* os_malloc(unsigned int sz)
{
    if (malloc_drv->malloc == NULL_PTR) return NULL_PTR;
    else return malloc_drv->malloc(sz);

}

void os_free(void *ptr)
{
    if (malloc_drv->free != NULL_PTR)
        malloc_drv->free(ptr);
}

unsigned int os_get_used_heap()
{
    if (malloc_drv->get_used == NULL_PTR) return 0;
    else return malloc_drv->get_used();
}

unsigned int os_get_free_heap()
{
    if (malloc_drv->get_free == NULL_PTR) return 0;
    else return malloc_drv->get_free();
}

unsigned int os_get_total_heap()
{
    if (malloc_drv->get_total == NULL_PTR) return 0;
    else return malloc_drv->get_total();
}

#endif // OS_MALLOC == OS_MALLOC_NONE
