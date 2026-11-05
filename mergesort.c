#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

/* ---- Frame size estimates (parameters + locals of each function) ---- */
/* mergeSortHelper(arr, temp, left, right, comparisons, depth) + local mid */
typedef struct {
    int *arr, *temp;
    int left, right, depth, mid;
    long long *comparisons;
} MergeSortFrameEstimate;

/* merge(arr, temp, left, mid, right, comparisons) + locals i, j, k */
typedef struct {
    int *arr, *temp;
    int left, mid, right;
    int i, j, k;
    long long *comparisons;
} MergeFrameEstimate;

static int maxDepth = 0;

/* Merge two sorted parts */
void merge(int arr[], int temp[],
           int left, int mid, int right,
           long long *comparisons)
{
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        (*comparisons)++;               /* value comparison */
        if (arr[i] <= arr[j]) temp[k++] = arr[i++];
        else                  temp[k++] = arr[j++];
    }
    while (i <= mid)   temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (i = left; i <= right; i++) arr[i] = temp[i];
}

/* Recursive Merge Sort */
void mergeSortHelper(int arr[], int temp[],
                     int left, int right,
                     long long *comparisons, int depth)
{
    if (depth > maxDepth) maxDepth = depth;

    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortHelper(arr, temp, left, mid, comparisons, depth + 1);
        mergeSortHelper(arr, temp, mid + 1, right, comparisons, depth + 1);
        merge(arr, temp, left, mid, right, comparisons);
    }
}

/* Max extra memory = temp array + recursion stack + one merge() frame
   (merge() is only called at the deepest point of the stack chain). */
void mergeSortMeasured(int arr[], int n,
                       long long *comparisons,
                       size_t *memoryBytes)
{
    int *temp = malloc((size_t)n * sizeof(int));
    if (temp == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    maxDepth = 0;
    *comparisons = 0;

    mergeSortHelper(arr, temp, 0, n - 1, comparisons, 1);

    *memoryBytes = (size_t)n * sizeof(int)
                 + (size_t)maxDepth * sizeof(MergeSortFrameEstimate)
                 + sizeof(MergeFrameEstimate);

    free(temp);
}