#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Swap function
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Partition function for QuickSort
int partition(int arr[], int low, int high) {
    int pivot = arr[high];  // pivot
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Parallel QuickSort function with hidden determinacy race
void parallel_quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        #pragma omp task shared(arr)
        parallel_quicksort(arr, low, pi - 1);

        #pragma omp task shared(arr)
        parallel_quicksort(arr, pi + 1, high);

        #pragma omp taskwait
    }
}

// Function to print an array
void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// Main function
int main() {
    int arr[] = {10, 7, 8, 9, 1, 5};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Unsorted array: ");
    print_array(arr, n);

    // Parallel QuickSort
    parallel_quicksort(arr, 0, n - 1);

    printf("Sorted array: ");
    print_array(arr, n);

    return 0;
}
