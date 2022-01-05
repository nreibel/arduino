#include "lists.h"
#include "os_mem.h"

/*
 * Recursive functions are NOT recommanded on devices with limited RAM/stack
 * This implementation is for educationnal purposes only
 */

#if OS_MALLOC

struct linked_list_prv_s {
    void *data;
    linked_list_t next;
};

linked_list_t linked_list_create()
{
    return os_calloc(sizeof(struct linked_list_prv_s));
}

linked_list_t linked_list_append(linked_list_t self, void *data)
{
    if (self == NULL_PTR)
    {
        return NULL_PTR;
    }
    else if (self->data == NULL_PTR)
    {
        self->data = data;
        return self;
    }
    else if (self->next != NULL_PTR)
    {
        return linked_list_append(self->next, data);
    }
    else
    {
        linked_list_t item = linked_list_create();

        if (item != NULL_PTR)
        {
            item->data = data;
            self->next = item;
        }

        return item;
    }
}

void* linked_list_get(linked_list_t self, unsigned int index)
{
    if (self == NULL_PTR) return NULL_PTR;
    else if (index == 0) return self->data;
    else if (self->next != NULL_PTR) return linked_list_get(self->next, index-1);
    else return NULL_PTR;
}

unsigned int linked_list_size(linked_list_t self)
{
    if (self == NULL_PTR) return 0;
    else if (self->data == NULL_PTR) return 0;
    else if (self->next != NULL_PTR) return 1 + linked_list_size(self->next);
    else return 1;

}

#endif // OS_MALLOC
