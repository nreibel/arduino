#include "lists.h"
#include "os_mem.h"

/*
 * Recursive functions are NOT recommanded on devices with limited RAM/stack
 * This implementation is for educationnal purposes only
 */

#if OS_MALLOC != OS_MALLOC_NONE

struct linked_list_prv_s {
    void * data;
    linked_list_t next;
};

linked_list_t linked_list_create(void)
{
    linked_list_t self = os_calloc(sizeof(*self));
    return self;
}

linked_list_t linked_list_push(linked_list_t self, void * data)
{
    if (self->next != NULL_PTR)
        return linked_list_push(self->next, data);

    const linked_list_t item = linked_list_create();

    if (item)
    {
        item->data = data;
        item->next = NULL_PTR;
        self->next = item;
        return self;
    }

    return NULL_PTR;
}

void * linked_list_peek(linked_list_t self)
{
    if (self->next)
        return self->next->data;

    return NULL_PTR;
}

void * linked_list_pop(linked_list_t self)
{
    void * ret = NULL_PTR;

    if (self->next)
    {
        ret = self->next->data;
        self->next = self->next->next;
    }

    return ret;
}

void * linked_list_get(linked_list_t self, unsigned int index)
{
    if (!self->next)
        return NULL_PTR;

    if (index > 0)
        return linked_list_get(self->next, index-1);

    return self->next->data;
}

unsigned int linked_list_size(linked_list_t self)
{
    if (!self->next)
        return 0;

    return 1 + linked_list_size(self->next);
}

bool linked_list_empty(linked_list_t self)
{
    return self->next == NULL_PTR;
}

#endif // OS_MALLOC
