#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

typedef struct {
    void* address;
    node* next;
} node;

typedef struct {
    node* head;
    node* tail;
    size_t block_size;
} linked_list;

linked_list* make_linked_list(size_t block_size);
node* ll_get(linked_list* list, size_t index);
void ll_remove(linked_list* list, size_t index);
void ll_add(linked_list* list, node* new_node);
void ll_add_new_node(linked_list* list, void* address);
void free_linked_list(linked_list* list);

#endif