#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/mman.h>
#include <stdlib.h> 
#include <stdio.h>
#include "binned_free_list.h"
#include "tools.h"
#include "linked_list.h"

static bool break_larger_blocks(binned_free_list* restrict list, int bin);  //takes in bin that needs block         returns false if no larger block
inline static void break_block(binned_free_list* restrict list, int upper_bin, int lower_bin);
static bool combine(binned_free_list* restrict list, int bin); //goes through lists bottom to bin trying to combine blocks (returns true if succesfully created a block of size bin)
static void combine_blocks(binned_free_list* restrict list, size_t index1, size_t index2, int bin);
//static bool get_more_memory(binned_free_list* restrict list, int bin); //gets more memory from OS (return false if mmap fails)

binned_free_list* make_binned_list(size_t num_bins, size_t min_bin_size) {
    if(min_bin_size < 3) return NULL;
    binned_free_list* list = (binned_free_list*)malloc(sizeof(binned_free_list));
    if (list == NULL) return NULL;
    list->bins = (linked_list**)malloc((num_bins) * sizeof(linked_list*));
    if (list->bins == NULL) {
        free(list);
        return NULL;
    }
    for(size_t i = 0; i < num_bins; ++i) {
        *(list->bins + i) = make_linked_list(1 << (i + min_bin_size));
    }
    list->min_bin_size = min_bin_size;
    list->num_bins = num_bins;
    return list;
}

void* bl_remove(binned_free_list* restrict list, size_t num_bytes) {
    num_bytes = round_up_to_power_of_2(num_bytes);
    int bin = log2_power_of_2(num_bytes) - list->min_bin_size;
    assert((*(list->bins + bin))->length == ll_verify_length(*(list->bins + bin)));
    void* address = ll_remove(*(list->bins + bin), 0);
    assert((*(list->bins + bin))->length == ll_verify_length(*(list->bins + bin)));
    if(address != NULL) return address;
    //otherwise no items in this and bin will have to break larger block and potentially combine and get more memory
    if(break_larger_blocks(list, bin)) { //returns true if succesfully broke down larger block
        assert((*(list->bins + bin))->length == ll_verify_length(*(list->bins + bin)));
        address = ll_remove(*(list->bins + bin), 0);
        assert((*(list->bins + bin))->length == ll_verify_length(*(list->bins + bin)));
        assert(address != NULL);
        return address;
    }
    assert((*(list->bins + bin))->length == ll_verify_length(*(list->bins + bin)));
    //otherwise no larger blocks to break down
    if(combine(list, bin)) { //First try to coalesce list and create at least 1 block in bin
        assert((*(list->bins + bin))->length == ll_verify_length(*(list->bins + bin)));
        address = ll_remove(*(list->bins + bin), 0);
        assert((*(list->bins + bin))->length == ll_verify_length(*(list->bins + bin)));
        assert(address != NULL);
        return address;
    }
    assert((*(list->bins + bin))->length == ll_verify_length(*(list->bins + bin)));
    //otherwise can not combine so have to get memory from OS
    // if(get_more_memory(list, bin)) {
    //     address = ll_remove(*(list->bins + bin), 0);
    //     assert(address != NULL);
    //     return address;
    // }
    return NULL;
}

void bl_add(binned_free_list* restrict list, void* restrict address, size_t num_bytes) {
    int bin = log2_power_of_2(num_bytes) - list->min_bin_size;
    linked_list* l_list = *(list->bins + bin);
    //printf("Linked List Lenth before: %d\n", l_list->length);
    assert(l_list->length == ll_verify_length(l_list) && "Before ADD");
    ll_add_new_node(*(list->bins + bin), address);
    //printf("Linked List Lenth after: %d\n", l_list->length);
    assert(l_list->length == ll_verify_length(l_list) && "After ADD");
    if(l_list->length > UNSORTED_LIMIT) {
        node* prev_node = l_list->head;
        for(int i = 0; i < UNSORTED_LIMIT - 1; ++i) {
            prev_node = prev_node->next;
        }
        node* sort_node = prev_node->next;
        node* walk_node = sort_node;
        while(walk_node->next != NULL) {
            if(sort_node->address < walk_node->next->address) {
                if(sort_node == walk_node) return;
                prev_node->next = sort_node->next;
                sort_node->next = walk_node->next;
                walk_node->next = sort_node;
                return;
            }
            walk_node = walk_node->next;
        }
        if(walk_node == sort_node) return;
        prev_node->next = sort_node->next;
        walk_node->next = sort_node;
        sort_node->next = NULL;
    }
}

static bool break_larger_blocks(binned_free_list* restrict list, int bin) {
    for(int upper_bin = bin + 1; upper_bin < list->num_bins; ++upper_bin) {
        if((*(list->bins + upper_bin))->head != NULL) {
            break_block(list, upper_bin, bin);
            return true;
        }
    }
    return false;
}

inline static void break_block(binned_free_list* restrict list, int upper_bin, int lower_bin) {
    assert((*(list->bins + upper_bin))->length == ll_verify_length(*(list->bins + upper_bin)));
    void* address = ll_remove(*(list->bins + upper_bin), 0);
    assert((*(list->bins + upper_bin))->length == ll_verify_length(*(list->bins + upper_bin)));
    assert(address != NULL);
    size_t num_bytes;
    for(int i = upper_bin - 1; i > lower_bin; --i) {
        num_bytes = 1 << (i + list->min_bin_size);
        bl_add(list, address, num_bytes);
        address = (void*)(((char*)address) + num_bytes);
    }
    num_bytes = 1 << (lower_bin + list->min_bin_size);
    bl_add(list, address, num_bytes);
    address = (void*)(((char*)address) + num_bytes);
    bl_add(list, address, num_bytes);
}

static bool combine(binned_free_list* restrict list, int bin) {
    for(int current_bin = 0; current_bin < bin; ++current_bin) {
        linked_list* l_list = *(list->bins + current_bin);
        if(l_list->head == NULL) continue;
        size_t unsorted_limit = l_list->length > UNSORTED_LIMIT ? UNSORTED_LIMIT : l_list->length - 1;
        size_t num_bytes = 1 << (current_bin + list->min_bin_size);
        node* current_node = l_list->head;
        node* compare_node;
        for(size_t i = 0; i < unsorted_limit; ++i) {
            if(current_node == NULL || current_node->next == NULL) break;
            compare_node = current_node->next;
            bool combined = false;
            for(size_t j = i + 1; j < l_list->length; ++j) {
                if(compare_node == NULL) break;
                if((current_node->address + num_bytes == compare_node->address) || (compare_node->address + num_bytes == current_node->address)) {
                    current_node = (j == i + 1) ? current_node->next->next : current_node->next;
                    combine_blocks(list, i, j, current_bin);
                    combined = true;
                    if(j < unsorted_limit) unsorted_limit--;
                    i--;
                    break;
                }
                compare_node = compare_node->next;
            }
            current_node = (combined) ? current_node : current_node->next;
        }
        if(current_node != NULL) {
            size_t i = unsorted_limit;
            while(current_node->next != NULL) {
                if(current_node->address + num_bytes == current_node->next->address) {
                    current_node = current_node->next->next;
                    combine_blocks(list, i, i + 1, current_bin);
                    if(current_node == NULL) break;
                    continue;
                }
                current_node = current_node->next;
                i++;
            }
        }
    }
    return (*(list->bins + bin))->head != NULL;
}

static void combine_blocks(binned_free_list* restrict list, size_t index1, size_t index2, int bin) {
    linked_list* l_list = *(list->bins + bin);
    assert(l_list->length == ll_verify_length(l_list));
    void* part1 = ll_remove(l_list, index1);
    assert(l_list->length == ll_verify_length(l_list));
    void* part2 = ll_remove(l_list, (index1 < index2) ? index2 - 1 : index2);
    assert(l_list->length == ll_verify_length(l_list));
    assert(part1 != NULL && part2 != NULL);
    size_t num_bytes = 1 << (bin + list->min_bin_size);
    if(((void*)((char*)part2 + num_bytes)) == part1) {
        void* temp = part1;
        part1 = part2;
        part2 = temp;
    }
    bl_add(list, part1, num_bytes << 1);
}

bool handle_more_memory(binned_free_list* restrict list, void* memory, size_t bin_bytes) {
    int bin = log2_power_of_2(bin_bytes) - list->min_bin_size;
    size_t num_bytes = 1 << (list->num_bins + list->min_bin_size);
    // void* memory = mmap(NULL, num_bytes, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    // if(memory == MAP_FAILED) return false;
    for(int i = list->num_bins - 1; i > bin; --i) {
        num_bytes = 1 << (i + list->min_bin_size);
        bl_add(list, memory, num_bytes);
        memory = (void*)((char*)memory + num_bytes);
    }
    num_bytes = 1 << (bin + list->min_bin_size);
    bl_add(list, memory, num_bytes);
    memory = (void*)((char*)memory + num_bytes);
    bl_add(list, memory, num_bytes);
    return true;
}

void free_binned_list(binned_free_list* restrict list) {
    if(list == NULL) return;

    for(size_t i = 0; i < list->num_bins; ++i) {
        free_linked_list(*(list->bins + i));
    }

    free(list->bins);
    free(list);
}