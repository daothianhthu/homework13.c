#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_SIZE 20000

void mergeSortMeasured(int arr[], int n, long long *comparisons, size_t *memoryBytes);
void quickSortFirst (int arr[], int low, int high, long long *comparisons);
void quickSortRandom(int arr[], int low, int high, long long *comparisons);
void quickSortMedian(int arr[], int low, int high, long long *comparisons);
size_t getQuickMemory(void);

/* High-resolution timer, returns milliseconds (same method for all algorithms) */
static double nowMs(void)
{
#ifdef _WIN32
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)f.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
#endif
}

static void generateData(int arr[], int n)
{
    /* rand() may be only 15 bits (Windows), so combine two calls */
    for (int i = 0; i < n; i++)
        arr[i] = (int)((((unsigned)rand() << 15) ^ (unsigned)rand()) % 100000);
}

static void copyArray(const int src[], int dst[], int n)
{
    for (int i = 0; i < n; i++) dst[i] = src[i];
}

static int isSorted(const int arr[], int n)
{
    for (int i = 1; i < n; i++)
        if (arr[i - 1] > arr[i]) return 0;
    return 1;
}

static void printRow(const char *name, long long cmp, double ms, size_t mem)
{
    printf("%-15s %-14lld %-11.3f %-14zu\n", name, cmp, ms, mem);
}

int main(void)
{
    int sizes[] = {1000, 2000, 4000, 8000, 16000, 20000};
    int numSizes = (int)(sizeof(sizes) / sizeof(sizes[0]));

    static int original[MAX_SIZE], mergeArr[MAX_SIZE], firstArr[MAX_SIZE],
               randomArr[MAX_SIZE], medianArr[MAX_SIZE];

    srand((unsigned int)time(NULL));

    for (int s = 0; s < numSizes; s++) {
        int n = sizes[s];
        long long cmp;
        double t0, t1;
        size_t mem;

        generateData(original, n);
        copyArray(original, mergeArr,  n);
        copyArray(original, firstArr,  n);
        copyArray(original, randomArr, n);
        copyArray(original, medianArr, n);

        printf("\nData Size : %d\n\n", n);
        printf("%-15s %-14s %-11s %-14s\n",
               "Algorithm", "Comparisons", "Time(ms)", "Memory(Bytes)");
        printf("--------------------------------------------------------\n");

        cmp = 0; mem = 0;
        t0 = nowMs();
        mergeSortMeasured(mergeArr, n, &cmp, &mem);
        t1 = nowMs();
        printRow("Merge Sort", cmp, t1 - t0, mem);
        int okMerge = isSorted(mergeArr, n);

        cmp = 0;
        t0 = nowMs();
        quickSortFirst(firstArr, 0, n - 1, &cmp);
        t1 = nowMs();
        printRow("Quick (First)", cmp, t1 - t0, getQuickMemory());
        int okFirst = isSorted(firstArr, n);

        cmp = 0;
        t0 = nowMs();
        quickSortRandom(randomArr, 0, n - 1, &cmp);
        t1 = nowMs();
        printRow("Quick (Random)", cmp, t1 - t0, getQuickMemory());
        int okRandom = isSorted(randomArr, n);

        cmp = 0;
        t0 = nowMs();
        quickSortMedian(medianArr, 0, n - 1, &cmp);
        t1 = nowMs();
        printRow("Quick (Median3)", cmp, t1 - t0, getQuickMemory());
        int okMedian = isSorted(medianArr, n);

        printf("\nSorted check: Merge=%s First=%s Random=%s Median3=%s\n",
               okMerge ? "OK" : "FAIL", okFirst ? "OK" : "FAIL",
               okRandom ? "OK" : "FAIL", okMedian ? "OK" : "FAIL");
    }
    return 0;
}