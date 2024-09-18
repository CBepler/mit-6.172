#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int reduce(int* A, int n) { //assume n is multiple of 8;
    int sum = 0;
    #pragma omp parallel shared(A)
    {
        int local_sum = 0;
        #pragma omp for
        for(int i = 0; i < n; i += 8){
            local_sum = A[i] + A[i + 1] + A[i + 2] + A[i + 3] + A[i + 4] + A[i + 5] + A[i + 6] + A[i + 7];
        }

        #pragma omp atomic
        sum += local_sum;
    }
    return sum;
}

int better_reduce(int* A, int n) {
    int sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for(int i = 0; i < n; ++i) {
        sum += *(A + i);
    }
    return sum;
}

int main(int argc, char const *argv[])
{
    int* A = malloc(sizeof(int) * 64);
    int n = 64;
    for(int i = 0; i < n; ++i) {
        *(A + i) = 1;
    }
    printf("%d\n", reduce(A, n));
    printf("%d", better_reduce(A, n));
    return 0;
}
