#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

typedef struct node {
    void* address;
    struct node* next;
} node;

typedef struct {
    node* head;
    node* tail;
    size_t block_size;
    size_t length;
} linked_list;

linked_list* make_linked_list(size_t block_size);
void* ll_get(linked_list* restrict list, size_t index);
void* ll_remove(linked_list* restrict list, size_t index);
void ll_add(linked_list* restrict list, node* new_node);
void ll_add_new_node(linked_list* restrict list, void* restrict address);
void free_linked_list(linked_list* restrict list);

#endif