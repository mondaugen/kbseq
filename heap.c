/*
 * A generalized heap
 */

#include "heap.h" 

#include <stdlib.h> 
#include <stddef.h> 
#include <stdio.h> 
#include <string.h> 

/* index of parent */
#define PARENT(i) (((size_t)i + 1)/2 - 1)

/* index of left child */
#define LEFT(i) (2*(size_t)i+1)

/* index of right child */
#define RIGHT(i) (LEFT(i)+1) 

/* swap keys */
#define SWAP(a,b) \
    void *__tmp; \
    __tmp = a; \
    a = b; \
    b = __tmp

void Heap_init(Heap *H, size_t maxsize, int (*cmp)(void*,void*))
{
    H->maxsize = maxsize;
    H->size = 0;
    H->cmp = cmp;
}

void Heap_heapify(Heap *H, size_t i)
{
    while (LEFT(i) < H->size) {
        size_t min;
        if (H->cmp(H->A[i],H->A[LEFT(i)])) {
            min = LEFT(i);
        } else {
            min = i;
        }
        if ((RIGHT(i) < H->size) &&
                (H->cmp(H->A[min],H->A[RIGHT(i)]))) {
            min = RIGHT(i);
        }
        if (min == i) {
            break;
        }
        SWAP(H->A[i], H->A[min]);
        i = min;
    }
}

HeapErr Heap_make_heap(Heap *H, void **A, size_t size)
{
    if (size > H->maxsize) {
        return HEAP_ESIZE;
    }
    H->size = size;
    memcpy(H->A, A, sizeof(void*) * size);
    size_t i = size;
    while (i--) {
        Heap_heapify(H, i);
    }
    return HEAP_ENONE;
}

HeapErr Heap_remove_min(Heap *H, void **key)
{
    if (!H->size) {
        return HEAP_EEMPTY;
    }
    if (!key) {
        return HEAP_EINVAL;
    }
    *key = H->A[0];
    H->A[0] = H->A[H->size - 1];
    H->size--;
    Heap_heapify(H, 0);
    return HEAP_ENONE;
}

static void float_up_last(Heap *H)
{
    size_t i = H->size - 1;
    while ((i > 0) && (H->cmp(H->A[PARENT(i)],H->A[i]))) {
        SWAP(H->A[PARENT(i)], H->A[i]);
        i = PARENT(i);
    }
}

HeapErr Heap_push(Heap *H, void *key)
{
    if (H->size == H->maxsize) {
        return HEAP_EFULL;
    }
    H->A[H->size] = key;
    H->size++;
    float_up_last(H);
    return HEAP_ENONE;
}
