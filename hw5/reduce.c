#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int reduce(int* A, int n) { //assume n is multiple of 8;
    int index = 0;
    #pragma omp parallel for shared(A, index)
    for(int i = 0; i < n; i += 8){
        A[index] = A[i] + A[i + 1] + A[i + 2] + A[i + 3] + A[i + 4] + A[i + 5] + A[i + 6] + A[i + 7];
        ++index;
    }
    int sum = 0;
    for(int i = 0; i < index; ++i) {
        sum += A[i];
    }
    return sum;
}

int main(int argc, char const *argv[])
{
    int* A = malloc(sizeof(int) * 64);
    int n = 64;
    for(int i = 0; i < n; ++i) {
        *(A + 1) = 1;
    }
    printf("%d", reduce(A, n));
    return 0;
}
