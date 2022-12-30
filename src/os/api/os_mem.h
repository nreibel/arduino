#ifndef __OS_MALLOC_API_H__
#define __OS_MALLOC_API_H__

#include "types.h"
#include "os_cfg.h"

typedef struct malloc_drv_s {
    void*        (*malloc)    (unsigned int sz);
    void*        (*calloc)    (unsigned int sz);
    void         (*free)      (void *ptr);
    unsigned int (*get_free)  (void);
    unsigned int (*get_used)  (void);
    unsigned int (*get_total) (void);
} const * malloc_drv_t;

void* os_malloc(unsigned int sz);
void* os_calloc(unsigned int sz);
void  os_free(void *ptr);

unsigned int os_get_used_heap();
unsigned int os_get_free_heap();
unsigned int os_get_total_heap();

#endif /* __OS_MALLOC_API_H__ */
