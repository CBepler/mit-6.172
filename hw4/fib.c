#include <omp.h>



int64_t fibonacci(int64_t n) {
    static int64_t fib[9] = {0, 1, 1, 2, 3, 5, 8, 13, 21};
    if(n < 9) {
        return fib[n];
    }
    int64_t x, y;
#pragma omp task shared(x, n)
    x = fibonacci(n - 1);
#pragma omp task shared(y, n)
    y = fibonacci(n - 2);
#pragma omp taskwait
    return x + y;
}