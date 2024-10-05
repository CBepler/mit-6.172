#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include "binned_free_list.h"
#include "tools.h"

binned_free_list* make_binned_list(size_t num_bins, size_t min_bin_size) {
    binned_free_list* list = (binned_free_list*)malloc(sizeof(binned_free_list));
    for(size_t i = 0; i < num_bins; ++i) {
        *(list->bins + i) = make_linked_list(1 << (i + min_bin_size));
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
    num_bytes = 1 << (lower_bin + list->min_bin_size);
    add(list, address, num_bytes);
    address = (void*)(((char*)address) + num_bytes);
    add(list, address, num_bytes);
}

static bool combine(binned_free_list* list, int bin) {
    for(int current_bin = 0; current_bin < bin; ++current_bin) {
        linked_list* l_list = *(list->bins + current_bin);
        size_t unsorted_limit = l_list->length > UNSORTED_LIMIT ? UNSORTED_LIMIT : l_list->length;
        size_t num_bytes = 1 << (current_bin + list->min_bin_size);
        node* current_node = l_list->head;
        for(size_t i = 0; i < unsorted_limit; ++i) {
            node* compare_node = current_node->next;
            bool combined = false;
            for(size_t j = i + 1; j < l_list->length; ++j) {
                if((current_node->address + num_bytes == compare_node->address) || (compare_node->address + num_bytes == current_node->address)) {
                    current_node = (j == i + 1) ? current_node->next->next : current_node->next;
                    combine_blocks(list, i, j, current_bin);
                    combined = true;
                    if(j < unsorted_limit) unsorted_limit--;
                    break;
                }
            }
            current_node = (combined) ? current_node : current_node->next;
        }
        for(size_t i = unsorted_limit; i < l_list->length - 1; ++i) {
            if(current_node->address + num_bytes == current_node->next->address) {
                current_node = current_node->next->next;
                combine_blocks(list, i, i + 1, current_bin);
                continue;
            }
            current_node = current_node->next;
        }
    }
    return (*(list->bins + bin))->head != NULL;
}

static void combine_blocks(binned_free_list* list, size_t index1, size_t index2, int bin) {
    linked_list* l_list = *(list->bins + bin);
    void* part1 = ll_remove(l_list, index1);
    void* part2 = ll_remove(l_list, (index1 < index2) ? index2 - 1 : index2);
    assert(part1 != NULL && part2 != NULL);
    size_t num_bytes = 1 << (bin + list->min_bin_size);
    if(((void*)((char*)part2 + num_bytes)) == part1) {
        void* temp = part1;
        part1 = part2;
        part2 = temp;
    }
    add(list, part1, num_bytes << 1);
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