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
void* remove(binned_free_list* list, size_t num_bytes);     //remove memory block from free list and return it
void add(binned_free_list* list, void* address, size_t num_bytes);   //add memory block to free list
static bool break_larger_blocks(binned_free_list* list, int bin);  //takes in bin that needs block         returns false if no larger block
static void break_block(binned_free_list* list, int upper_bin, int lower_bin);
static bool combine(binned_free_list* list, int bin); //goes through lists bottom to bin trying to combine blocks (returns true if succesfully created a block of size bin)
static bool get_more_memory(binned_free_list* list, int bin); //gets more memory from OS (return false if mmap fails)
void free_binned_list(binned_free_list* list);



#endif