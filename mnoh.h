#ifndef MNOH_H
#define MNOH_H 

#include "err.h" 
#include "heap.h" 
#include "midi.h"

#define VVVV_MNOH_ARRAY_SIZE 128

typedef struct vvvv_mnoh_elem_t {
    vvvv_midipckt_t packet;
    size_t curidx; /* Index in heap */
} vvvv_mnoh_elem_t;

typedef struct vvvv_mnoh_t {
    vvvv_mnoh_elem_t elems[VVVV_MNOH_ARRAY_SIZE];
    Heap heap;
} vvvv_mnoh_t;

/* Gives the size in bytes of a mnoh that can hold n elements */
#define VVVV_MNOH_SIZE(n) \
    (sizeof(vvvv_mnoh_t) + sizeof(void*) * n)

#define VVVV_MNOH_ALLOC_STACK(varname) \
    char varname ## _data__[VVVV_MNOH_SIZE(VVVV_MNOH_ARRAY_SIZE)]; \
    varname = (vvvv_mnoh_t*)varname ## _data__;

void vvvv_mnoh_init(vvvv_mnoh_t *mnoh);
vvvv_err_t vvvv_mnoh_insert(vvvv_mnoh_t *mnoh, vvvv_midipckt_t *p);

#endif /* MNOH_H */
