#ifndef __OS_MALLOC_API_H__
#define __OS_MALLOC_API_H__

#include "types.h"
#include "os_cfg.h"

#if defined OS_MALLOC == ON

void* os_malloc(unsigned int sz);
void* os_calloc(unsigned int sz);
void  os_free(void *ptr);

unsigned int os_get_used_heap();
unsigned int os_get_free_heap();
unsigned int os_get_total_heap();

unsigned int os_get_used_chunks();
unsigned int os_get_free_chunks();
unsigned int os_get_total_chunks();

#endif // OS_MALLOC

#endif /* __OS_MALLOC_API_H__ */
