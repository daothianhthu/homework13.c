#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

/* ---- Frame size estimates (parameters + locals of each function) ---- */
/* quick*Helper(arr, low, high, comparisons, depth) + locals pivotIndex, pi */
typedef struct {
    int *arr;
    int low, high, depth;
    long long *comparisons;
    int pivotIndex, pi;
} QuickSortFrameEstimate;

/* partition(arr, low, high, comparisons) + locals pivot, i, j */
typedef struct {
    int *arr;
    int low, high;
    long long *comparisons;
    int pivot, i, j;
} PartitionFrameEstimate;

static int maxDepth = 0;

static void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

static int randomPivot(int low, int high)
{
    return low + rand() % (high - low + 1);
}

/* Median of Three: 3 value comparisons are counted */
static int medianOfThree(int arr[], int low, int high,
                         long long *comparisons)
{
    int mid = low + (high - low) / 2;
    int a = arr[low], b = arr[mid], c = arr[high];

    (*comparisons)++; if (a > b) swap(&a, &b);
    (*comparisons)++; if (b > c) swap(&b, &c);
    (*comparisons)++; if (a > b) swap(&a, &b);

    if (b == arr[low]) return low;
    if (b == arr[mid]) return mid;
    return high;
}

/* Lomuto partition, pivot = arr[high] */
static int partition(int arr[], int low, int high,
                     long long *comparisons)
{
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        (*comparisons)++;
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

/* pivotType: 0 = first, 1 = random, 2 = median of three */
static void quickHelper(int arr[], int low, int high,
                        long long *comparisons, int depth, int pivotType)
{
    if (depth > maxDepth) maxDepth = depth;

    if (low < high) {
        int pivotIndex;
        if (pivotType == 0)      pivotIndex = low;
        else if (pivotType == 1) pivotIndex = randomPivot(low, high);
        else                     pivotIndex = medianOfThree(arr, low, high, comparisons);

        swap(&arr[pivotIndex], &arr[high]);
        int pi = partition(arr, low, high, comparisons);

        quickHelper(arr, low, pi - 1, comparisons, depth + 1, pivotType);
        quickHelper(arr, pi + 1, high, comparisons, depth + 1, pivotType);
    }
}

void quickSortFirst(int arr[], int low, int high, long long *comparisons)
{
    maxDepth = 0;
    quickHelper(arr, low, high, comparisons, 1, 0);
}

void quickSortRandom(int arr[], int low, int high, long long *comparisons)
{
    maxDepth = 0;
    quickHelper(arr, low, high, comparisons, 1, 1);
}

void quickSortMedian(int arr[], int low, int high, long long *comparisons)
{
    maxDepth = 0;
    quickHelper(arr, low, high, comparisons, 1, 2);
}

/* Max extra memory = recursion stack + one partition() frame
   (no extra array is allocated). Same formula style as merge sort. */
size_t getQuickMemory(void)
{
    return (size_t)maxDepth * sizeof(QuickSortFrameEstimate)
         + sizeof(PartitionFrameEstimate);
}