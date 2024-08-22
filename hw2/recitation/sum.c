// Copyright (c) 2012 MIT License by 6.172 Staff

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint32_t data_t;
const int U = 10000000;   // size of the array. 10 million vals ~= 40MB
const int N = 100000000;  // number of searches to perform

int main() {
  data_t* data = (data_t*) malloc(U * sizeof(data_t));
  if (data == NULL) {
    free(data);
    printf("Error: not enough memory\n");
    exit(-1);
  }

  // fill up the array with sequential (sorted) values.
  int i;
  for (i = 0; i < U; i++) {
    data[i] = i;
  }

  printf("Allocated array of size %d\n", U);
  printf("Summing %d random values...\n", N);

  data_t val = 0;
  data_t seed = 42;
  for (i = 0; i < N; i++) {
    int l = rand_r(&seed) % U;
    val = (val + data[l]);  //All the data read misses happen here because you are accessing a random portion of the array on each cycle
    //That means that there is no utilization of the cache spatial locality since you are randomly jumping between parts of the array. You can decrease the amount of data 
    //read misses by going through the array in order to utilize spatial locality or you can decrease the size of the array so that a larger percent of the array
    //can be store in the caches lowing cache misses.
  }

  free(data);
  printf("Done. Value = %d\n", val);
  return 0;
}
