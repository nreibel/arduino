#ifndef __OS_LISTS_API_H__
#define __OS_LISTS_API_H__

#include "types.h"
#include "os_cfg.h"

typedef struct linked_list_prv_s * linked_list_t;

linked_list_t linked_list_create();

linked_list_t linked_list_append(linked_list_t list, void *data);
void* linked_list_get(linked_list_t list, unsigned int index);

#endif /* __OS_LISTS_API_H__ */
