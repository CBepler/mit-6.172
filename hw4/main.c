#include <stdio.h>
#include "fasttime.h"

#define N 40

int fibonacci(int n);

int main() {

    fasttime_t startTime = gettime();
    int fib = fibonacci(N);
    fasttime_t endTime = gettime();

    double elapsedTime = tdiff(startTime, endTime);
    printf("Elapsed time: %.9f seconds to find the %d fibonacci number: %d", elapsedTime, N, fib);
}