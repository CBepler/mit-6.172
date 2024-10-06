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
    if(list == NULL || list->head == NULL || index >= list->length) return NULL;
    node* found;
    void* address;
    if(index == 0) {
        found = list->head;
        list->head = list->head->next;
        if(list->head == NULL) {
            list->tail = NULL;
        }
    }
    else {
        node* item = list->head;
        for(size_t i = 0; i < index - 1; ++i) {
            item = item->next;
        }
        found = item->next;
        item->next = found->next;
        if(item->next == NULL) {
            list->tail = item;
        }
    }
    address = found->address;
    free(found);
    list->length--;
    return address;
}

void ll_add(linked_list* restrict list, node* new_node) {
    if (list == NULL || new_node == NULL) return;
    list->length++;
    if(list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        new_node->next = NULL;
        return;
    }
    new_node->next = list->head;
    list->head = new_node;
}

void ll_add_new_node(linked_list* restrict list, void* restrict address) {
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

size_t ll_verify_length(linked_list* list) {
    size_t actual_length = 0;
    node* current = list->head;
    while (current != NULL) {
        actual_length++;
        current = current->next;
    }
    return actual_length;
}
