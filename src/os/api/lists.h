#ifndef __OS_LISTS_API_H__
#define __OS_LISTS_API_H__

#include "types.h"
#include "os_cfg.h"

#if OS_MALLOC

typedef struct linked_list_prv_s * linked_list_t;

linked_list_t linked_list_create();
linked_list_t linked_list_append(linked_list_t list, void *data);
void* linked_list_get(linked_list_t list, unsigned int index);
unsigned int linked_list_size(linked_list_t list);

#endif // OS_MALLOC

#endif /* __OS_LISTS_API_H__ */
