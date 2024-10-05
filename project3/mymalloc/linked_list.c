#include <stddef.h>
#include "linked_list.h"


linked_list* make_linked_list(size_t block_size) {
    linked_list* list = (linked_list*)malloc(sizeof(linked_list));
    list->block_size = block_size;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

node* ll_get(linked_list* list, size_t index) {
    node* item = list->head;
    for(size_t i = 0; i < index; ++i) {
        item = item->next;
    }
    return item;
}

void ll_remove(linked_list* list, size_t index) {
    if(index == 0) {
        list->head = list->head->next;
        return;
    }
    node* item = list->head;
    for(size_t i = 0; i < index - 1; ++i) {
        item = item->next;
    }
    if(item->next->address == list->tail->address) {
        item->next = NULL;
        list->tail = item;
    }
    item->next = item->next->next;
}

void ll_add(linked_list* list, node* new_node) {
    if(list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        return;
    }
    list->tail->next = new_node;;
    list->tail = new_node;
}

void ll_add_new_node(linked_list* list, void* address) {
    node* new = (node*)malloc(sizeof(node));
    new->address = address;
    new->next = NULL;
    add(list, new);
}

void free_linked_list(linked_list* list) {
    if(list == NULL) return;

    node* current = list->head;
    while(current != NULL) {
        node* next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
