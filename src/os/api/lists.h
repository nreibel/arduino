#ifndef __OS_LISTS_API_H__
#define __OS_LISTS_API_H__

#include "types.h"
#include "os_cfg.h"

#if OS_MALLOC != OS_MALLOC_NONE

typedef struct linked_list_prv_s * linked_list_t;

linked_list_t   linked_list_create  (void);
linked_list_t   linked_list_push    (linked_list_t list, void * data);
void *          linked_list_get     (linked_list_t list, unsigned int index);
void *          linked_list_pop     (linked_list_t list);
void *          linked_list_peek    (linked_list_t list);
unsigned int    linked_list_size    (linked_list_t list);
bool            linked_list_empty   (linked_list_t list);

#endif // OS_MALLOC

#endif /* __OS_LISTS_API_H__ */
