#include <stdio.h>
#include "fasttime.h"

#define N 45

int fibonacci(int n);

int main() {

    fasttime_t startTime = gettime();
    int fib = fibonacci(N);
    fasttime_t endTime = gettime();

    double elapsedTime = tdiff(startTime, endTime);
    printf("Elapsed time: %.4f seconds to find the %d fibonacci number: %d", elapsedTime, N, fib);
}