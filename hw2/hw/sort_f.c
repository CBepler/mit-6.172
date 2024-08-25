/**
 * Copyright (c) 2012 MIT License by 6.172 Staff
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


#include "./util.h"
#include "sort.h"

// Function prototypes
static inline void merge_f(data_t* A, int p, int q, int r, data_t* left);
static inline void copy_f(data_t* source, data_t* dest, int n);

// A basic merge sort routine that sorts the subarray A[p..r]
void sort_f(data_t* A, int p, int r) {
  assert(A);
  int top = 0;
  static data_t* left = NULL;
  if(left == NULL) {
    left = 0;
    top = 1;
    mem_alloc(&left, ((r - p) / 2)  + 1);
  }
  if (p < r) {
    if(r - p <= 10) {
      isort(A + p, A + r);
      return;
    }
    int q = (p + r) / 2;
    sort_f(A, p, q);
    sort_f(A, q + 1, r);
    merge_f(A, p, q, r, left);
  }
  if(top == 1) {
      mem_free(&left);
  }
}

// A merge routine. Merges the sub-arrays A [p..q] and A [q + 1..r].
// Uses two arrays 'left' and 'right' in the merge operation.
static inline void merge_f(data_t* A, int p, int q, int r, data_t* left) {
  assert(A);
  assert(p <= q);
  assert((q + 1) <= r);
  int n1 = q - p + 1;
  int n2 = r - q;

  data_t* right = A + n1;
  if (left == NULL || right == NULL) {
    mem_free(&left);
    return;
  }

  copy_f(&(A[p]), left, n1);
  *(left + n1) = UINT_MAX;

  int i = 0;
  int j = 0;
  int k = 0;

  for (k = p; k <= r; k++) {
    if (*(left + i) <= *(right + j)) {
      *(A + k) = *(left + i);
      i++;
    } else {
      *(A + k) = *(right + j);
      j++;
      if(j > n2) {
        break;
      }
    }
  }
  while(i < n1) {
    *(A + k) = *(left + i);
    i++;
    k++;
  }
}

static inline void copy_f(data_t* source, data_t* dest, int n) {
  assert(dest);
  assert(source);

  for (int i = 0 ; i < n ; i++) {
    *(dest + i) = *(source + i);
  }
}






