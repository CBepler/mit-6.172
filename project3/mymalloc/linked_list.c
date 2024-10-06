#include <stddef.h>
#include <stdlib.h> 
#include "linked_list.h"


linked_list* make_linked_list(size_t block_size) {
    linked_list* list = (linked_list*)malloc(sizeof(linked_list));
    list->block_size = block_size;
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    return list;
}

void* ll_get(linked_list* restrict list, size_t index) {
    node* item = list->head;
    for(size_t i = 0; i < index; ++i) {
        item = item->next;
        if(item == NULL) return NULL;
    }
    return item->address;
}

void* ll_remove(linked_list* restrict list, size_t index) {
    if(list->head == NULL) return NULL;
    node* found;
    if(index == 0) {
        found = list->head;
        list->head = list->head->next;
        list->length--;
        return found->address;
    }
    node* item = list->head;
    for(size_t i = 0; i < index - 1; ++i) {
        item = item->next;
        if(item == NULL) return NULL;
    }
    if(item->next == NULL) return NULL;
    found = item->next;
    if(item->next->address == list->tail->address) {
        item->next = NULL;
        list->tail = item;
        list->length--;
        return found->address;
    }
    item->next = item->next->next;
    list->length--;
    return found->address;
}

void ll_add(linked_list* restrict list, node* new_node) {
    list->length++;
    if(list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        return;
    }
    new_node->next = list->head;
    list->head = new_node;
}

void ll_add_new_node(linked_list* restrict list, void* address) {
    node* new = (node*)malloc(sizeof(node));
    new->address = address;
    new->next = NULL;
    ll_add(list, new);
}

void free_linked_list(linked_list* restrict list) {
    if(list == NULL) return;

    node* current = list->head;
    while(current != NULL) {
        node* next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
