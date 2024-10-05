#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include "binned_free_list.h"
#include "tools.h"

binned_free_list* make_binned_list(size_t num_bins, size_t min_bin_size) {
    binned_free_list* list = (binned_free_list*)malloc(sizeof(binned_free_list));
    for(size_t i = 0; i < num_bins; ++i) {
        *(list->bins + i) = make_linked_list(2 << i + min_bin_size);
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
}

static bool break_larger_blocks(binned_free_list* list, int bin) {

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