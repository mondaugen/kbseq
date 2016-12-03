#include "heap.h" 

#include <stdlib.h>
#include <string.h> 
#include <time.h> 
#include <assert.h> 
#include <stdio.h> 

static void print_int_heap(Heap *H)
{
    size_t i;
    for (i = 0; i < H->size; i++) {
        printf("%d ", *(int*)H->A[i]);
    }
    printf("\n");
}

/* index of parent */
#define PARENT(i) (((size_t)i + 1)/2 - 1)

/* index of left child */
#define LEFT(i) (2*(size_t)i+1)

/* index of right child */
#define RIGHT(i) (LEFT(i)+1) 

/* This comparison function makes a min heap where equal elements are allowed */
static int int_cmp(void *a, void *b)
{
    if (*((int*)a) >= *((int*)b)) {
        return 1;
    }
    return 0;
}

static int chk_is_heap(Heap *H, size_t i)
{
    int passed = 1;
    if (LEFT(i) < H->size) {
        passed &= int_cmp(H->A[LEFT(i)],H->A[i]);
        passed &= chk_is_heap(H,LEFT(i));
    }
    if (RIGHT(i) < H->size) {
        passed &= int_cmp(H->A[RIGHT(i)],H->A[i]);
        passed &= chk_is_heap(H,RIGHT(i));
    }
    return passed;
}

static int chk_is_sorted(int *A, size_t size)
{
    int passed = 1;
    if (size <= 1) {
        return 1;
    }
    while (size-- > 1) {
        passed &= int_cmp((void*)&A[size],(void*)&A[size-1]);
    }
    return passed;
}

#define ARRAY_LEN 1000

int main (void)
{

    int A[ARRAY_LEN];
    int *pA[ARRAY_LEN];
    int B[ARRAY_LEN];
    size_t i;
    int passed = 1;
    srandom(time(NULL));
    for (i = 0; i < ARRAY_LEN; i++) {
        A[i] = random() % 100;
        pA[i] = &A[i];
    }
    Heap *H;
    HEAP_ALLOC(H, ARRAY_LEN, int_cmp);
    Heap_make_heap(H, (void**)pA, ARRAY_LEN);
//    print_int_heap(H);
    passed &= chk_is_heap(H, 0);
    assert(passed);
    memset(B, 0, sizeof(int)*ARRAY_LEN);
    i = 0;
    while (H->size) {
        int *key;
        Heap_remove_min(H, (void*)&key);
        B[i++] = *key;
    }
    passed &= chk_is_sorted(B,ARRAY_LEN);
    assert(passed);
    i = 0;
    while (1) {
        HeapErr err;
        err = Heap_push(H, (void*)pA[i++]);
        if (err) {
            break;
        }
    }
    passed &= chk_is_heap(H, 0);
    assert(passed);
    return -1 * (!passed);
}


