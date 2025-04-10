/**
 * Copyright (c) 2015 MIT License by 6.172 Staff
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "./allocator_interface.h"
#include "./memlib.h"
#include "./binned_free_list.h"
#include "./tools.h"

// Don't call libc malloc!
#define malloc(...) (USE_MY_MALLOC)
#define free(...) (USE_MY_FREE)
#define realloc(...) (USE_MY_REALLOC)

// All blocks must have a specified minimum alignment.
// The alignment requirement (from config.h) is >= 8 bytes.
#ifndef ALIGNMENT
  #define ALIGNMENT 8
#endif

// Rounds up to the nearest multiple of ALIGNMENT.
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

// The smallest aligned size that will hold a size_t value.
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// check - This checks our invariant that the size_t header before every
// block points to either the beginning of the next block, or the end of the
// heap.
int my_check() {
  char* p;
  char* lo = (char*)mem_heap_lo();
  char* hi = (char*)mem_heap_hi() + 1;
  size_t size = 0;

  p = lo;
  while (lo <= p && p < hi) {
    size = ALIGN(*(size_t*)p + SIZE_T_SIZE);
    p += size;
  }

  if (p != hi) {
    printf("Bad headers did not end at heap_hi!\n");
    printf("heap_lo: %p, heap_hi: %p, size: %lu, p: %p\n", lo, hi, size, p);
    return -1;
  }

  return 0;
}

#define NUM_BINS 22
#define MIN_BIN_SIZE 3

binned_free_list* list = NULL;

// init - Initialize the malloc package.  Called once before any other
// calls are made.  Since this is a very simple implementation, we just
// return success.
int my_init() {
  list = make_binned_list(NUM_BINS, MIN_BIN_SIZE);
  return 0;
}

//  malloc - Allocate a block by incrementing the brk pointer.
//  Always allocate a block whose size is a multiple of the alignment.
void* my_malloc(size_t size) {
  // We allocate a little bit of extra memory so that we can store the
  // size of the block we've allocated.  Take a look at realloc to see
  // one example of a place where this can come in handy.
  size_t aligned_size = ALIGN(size + SIZE_T_SIZE);
  aligned_size = round_up_to_power_of_2(aligned_size);


  void* address = bl_remove(list, aligned_size);

  if(address == NULL) { //equals NULL means bin list needs more memory
    void* p = mem_sbrk(2 * aligned_size);
    if(p == (void*) - 1) return NULL;
    address = (void*)((char*)p + aligned_size);
    bl_add(list, p, aligned_size);
    //handle_more_memory(list, (void*)((char*)p + aligned_size), aligned_size);
    //address = bl_remove(list, aligned_size);
  }


  // We store the size of the block we've allocated in the first
  // SIZE_T_SIZE bytes.
  *(size_t*)address = aligned_size;

  // Then, we return a pointer to the rest of the block of memory,
  // which is at least size bytes long.  We have to cast to uint8_t
  // before we try any pointer arithmetic because voids have no size
  // and so the compiler doesn't know how far to move the pointer.
  // Since a uint8_t is always one byte, adding SIZE_T_SIZE after
  // casting advances the pointer by SIZE_T_SIZE bytes.
  return (void*)((char*)address + SIZE_T_SIZE);
}

// free - Freeing a block does nothing.
void my_free(void* ptr) {
  ptr = (void*)((char*)ptr - SIZE_T_SIZE);
  size_t size = *(size_t*)ptr; 
  bl_add(list, ptr, size);
}

// realloc - Implemented simply in terms of malloc and free
void* my_realloc(void* ptr, size_t size) {
  if (ptr == NULL) {
    return my_malloc(size);
  }
  if (size == 0) {
    my_free(ptr);
    return NULL;
  }
  void* newptr;
  void* front_ptr = (void*)((char*)ptr - SIZE_T_SIZE);
  size_t original_size = *(size_t*)front_ptr; 

  size_t new_size = ALIGN(size + SIZE_T_SIZE);
  size_t space_needed = round_up_to_power_of_2(new_size);

  if(original_size == space_needed) return ptr;

  int original_power = log2_power_of_2(original_size);
  int new_power = log2_power_of_2(space_needed);
  if(new_power < original_power) {
    while(new_power < original_power) {
      my_free((void*)((char*)front_ptr + (1 << (original_power - 1))));
      --original_power;
    }
    *(size_t*)front_ptr = space_needed;
    return ptr;
  }

  // Allocate a new chunk of memory, and fail if that allocation fails.
  newptr = my_malloc(size);
  if (NULL == newptr) {
    return NULL;
  }

  size_t copy_size = (size > original_size) ? original_size : size;

  // This is a standard library call that performs a simple memory copy.
  memcpy(newptr, ptr, copy_size);

  // Release the old block.
  my_free(ptr);

  // Return a pointer to the new block.
  return newptr;
}
