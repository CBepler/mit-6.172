#ifndef BINNED_H
#define BINNED_H

#include <stddef.h>
#include <stdbool.h>
#include "linked_list.h"

typedef struct {
    linked_list** bins;
    size_t num_bins;
    size_t min_bin_size;
} binned_free_list;

binned_free_list* make_binned_list(size_t num_bins, size_t min_bin_size);
void* remove(binned_free_list* list, size_t num_bytes);     //remove memory block from free list and return it
void add(binned_free_list* list, void* address, size_t num_bytes);   //add memory block to free list
bool break_larger_blocks(binned_free_list* list, size_t num_bytes);  //takes in number of bytes needed to know which bin to start from         returns false if no larger block
void combine(binned_free_list* list); //goes through lists bottom to top trying to combine blocks
bool get_more_memory(binned_free_list* list); //gets more memory from OS (return false if mmap fails)
void free_binned_list(binned_free_list* list);



#endif