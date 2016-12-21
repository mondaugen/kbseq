#ifndef SQ_H
#define SQ_H 

/* Generic sequence.
 *
 * Elements organized by vertical and horizontal organizing principles.
 * Each element wrapped in data type supplying functions for finding vertical
 * and horizontal location of element in sequence.
 *
 * How do I remove an element?
 * Simple call MMDLList_remove on the vvvv_sq_lst_elm_t item you want to remove.
 */

#include "tmstmp.h"
#include "mm_dllist.h" 

typedef struct vvvv_sq_lst_elm_vt_t {
    /* How to get the timestamp of the datum */
    vvvv_tmstmp_t (get_ts*)(void *);
    /* How to get the "vertical" ordering of the datum */
    vvvv_ord_t    (get_ord*)(void *);
    /* How to free the datum. Can just do nothing if you don't want it to */
    void          (free*)(void*);
} vvvv_sq_lst_elm_vt_t;

typedef struct vvvv_sq_lst_elm_t {
    MMDLList super;
    vvvv_sq_lst_elm_vt_t *vt;
    void *dat;
} vvvv_sq_lst_elm_t;

typedef struct vvvv_sq_elm_t {
    MMDLList lst_head;
} vvvv_sq_elm_t;

typedef struct vvvv_sq_t {
    /* The time between two adjacent indices in sq_elms */
    vvvv_tmstmp_t tick_dur; 
    /* Sequence length as number of elements in sq_elms */
    size_t len; 
    vvvv_sq_elm_t *sq_elms[1];
} vvvv_evnt_sq_t;

/* Callback information passed to vvvv_evnt_sq_map_range */
typedef struct vvvv_sq_mp_rng_t {
    void (*func)(vvvv_sq_t *, // The event sequence this was called on.
                 size_t, // The index of the event in the sequence.
                 void *, // The datum 
                 void *); // The auxiliary field in this structure.
    void *aux;
} vvvv_sq_mp_rng_t;

void vvvv_sq_map_range(vvvv_sq_t *seq,
                       vvvv_tmstmp_t tmin,
                       vvvv_tmstmp_t tmax,
                       vvvv_sq_mp_rng_t *cb);
#endif /* SQ_H */
