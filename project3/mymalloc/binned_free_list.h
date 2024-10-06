#ifndef BINNED_H
#define BINNED_H

#include <stddef.h>
#include <stdbool.h>
#include "linked_list.h"

#define UNSORTED_LIMIT 12     //adjustable boundary where first elements of bin are unsorted to improve cache locality and the rest are sorted to allow quick coalescing

typedef struct {
    linked_list** bins;
    size_t num_bins;
    size_t min_bin_size;
} binned_free_list;

binned_free_list* make_binned_list(size_t num_bins, size_t min_bin_size);
void* bl_remove(binned_free_list* restrict list, size_t num_bytes);     //remove memory block from free list and return it
void bl_add(binned_free_list* restrict list, void* restrict address, size_t num_bytes);   //add memory block to free list
void free_binned_list(binned_free_list* restrict list);
bool handle_more_memory(binned_free_list* restrict list, void* memory, size_t bin_bytes);



#endif