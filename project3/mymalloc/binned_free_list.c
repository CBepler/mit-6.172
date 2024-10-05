#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include "binned_free_list.h"
#include "tools.h"

binned_free_list* make_binned_list(size_t num_bins, size_t min_bin_size) {
    binned_free_list* list = (binned_free_list*)malloc(sizeof(binned_free_list));
    for(size_t i = 0; i < num_bins; ++i) {
        *(list->bins + i) = make_linked_list(1 << i + min_bin_size);
    }
    list->min_bin_size = min_bin_size;
    list->num_bins = num_bins;
}

void* remove(binned_free_list* list, size_t num_bytes) {
    num_bytes = round_up_to_power_of_2(num_bytes);
    int bin = log2_power_of_2(num_bytes) - list->min_bin_size;
    void* address = ll_remove(*(list->bins + bin), 0);
    if(address != NULL) return address;
    //otherwise no items in this and bin will have to break larger block and potentially combine and get more memory
    if(break_larger_blocks(list, bin)) { //returns true if succesfully broke down larger block
        address = ll_remove(*(list->bins + bin), 0);
        assert(address != NULL);
        return address;
    }
    //otherwise no larger blocks to break down
    if(combine(list, bin)) { //First try to coalesce list and create at least 1 block in bin
        address = ll_remove(*(list->bins + bin), 0);
        assert(address != NULL);
        return address;
    }
    //otherwise can not combine so have to get memory from OS
    if(get_more_memory(list, bin)) {
        address = ll_remove(*(list->bins + bin), 0);
        assert(address != NULL);
        return address;
    }
    return NULL;
}

void add(binned_free_list* list, void* address, size_t num_bytes) {
    int bin = log2_power_of_2(num_bytes) - list->min_bin_size;
    ll_add_new_node(*(list->bins + bin), address);
    if((*(list->bins + bin))->length > UNSORTED_LIMIT) {
        linked_list* l_list = *(list->bins + bin);
        node* sort_node = l_list->head;
        for(int i = 0; i < UNSORTED_LIMIT; ++i) {
            sort_node = sort_node->next;
        }
        for(int i = UNSORTED_LIMIT + 1; i < l_list->length; ++i) {
            if(sort_node->address > sort_node->next->address) {
                node* next = sort_node->next;
                sort_node->next = sort_node->next->next;
                next->next = sort_node;
                break;
            }
        }
    }
}

static bool break_larger_blocks(binned_free_list* list, int bin) {
    for(int upper_bin = bin + 1; upper_bin < list->num_bins; ++upper_bin) {
        if((*(list->bins + upper_bin))->head != NULL) {
            break_block(list, upper_bin, bin);
            return true;
        }
    }
    return false;
}

static void break_block(binned_free_list* list, int upper_bin, int lower_bin) {
    void* address = ll_remove(*(list->bins + upper_bin), 0);
    assert(address != NULL);
    size_t num_bytes;
    for(int i = upper_bin - 1; i > lower_bin; --i) {
        num_bytes = 1 << i + list->min_bin_size;
        add(list, address, num_bytes);
        address = (void*)(((char*)address) + num_bytes);
    }
    num_bytes = 1 << lower_bin + list->min_bin_size;
    add(list, address, num_bytes);
    address = (void*)(((char*)address) + num_bytes);
    add(list, address, num_bytes);
}

static bool combine(binned_free_list* list, int bin) {

}

static bool get_more_memory(binned_free_list* list, int bin) {

}

void free_binned_list(binned_free_list* list) {
    if(list == NULL) return;

    for(size_t i = 0; i < list->num_bins; ++i) {
        free_linked_list(*(list->bins + i));
    }

    free(list);
}