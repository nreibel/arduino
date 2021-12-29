#include "lists.h"
#include "os_mem.h"

struct linked_list_prv_s {
    void *data;
    linked_list_t next;
};

linked_list_t linked_list_create()
{
    linked_list_t self = os_malloc(sizeof(struct linked_list_prv_s));

    if (self != NULL_PTR)
    {
        self->data = NULL_PTR;
        self->next = NULL_PTR;
    }

    return self;
}

linked_list_t linked_list_append(linked_list_t self, void *data)
{
    linked_list_t current = self;

    if (current == NULL_PTR)
        return NULL_PTR;

    // Check for empty list
    if (current->data == NULL_PTR)
    {
        current->data = data;
        return current;
    }

    // Find last element
    while(current->next != NULL_PTR)
        current = current->next;

    // Create new item
    linked_list_t item = linked_list_create();
    if (item == NULL_PTR) return NULL_PTR;
    item->data = data;
    item->next = NULL_PTR;

    // Chain items
    current->next = item;

    return item;
}

void* linked_list_get(linked_list_t self, unsigned int index)
{
    linked_list_t current = self;

    if (current == NULL_PTR)
        return NULL_PTR;

    for(unsigned int i = 0 ; i < index ; i++)
    {
        if (current->next == NULL_PTR)
            return NULL_PTR;

        current = current->next;
    }

    return current->data;
}
