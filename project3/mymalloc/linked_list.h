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
node* get(linked_list* list, int index);
void remove(linked_list* list, int index);
void add(linked_list* list, node* new_node);
void add_new_node(linked_list* list, void* address);

#endif