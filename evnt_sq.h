#ifndef EVNT_SQ_H
#define EVNT_SQ_H 

#include "common.h" 
#include "mm_dllist.h" 
#include "evnt.h" 

typedef struct vvvv_evnt_lst_elm_t {
    MMDLList head;
    vvvv_evnt_t *evnt;
} vvvv_evnt_lst_elm_t;

typedef struct vvvv_evnt_lst_t {
    vvvv_evnt_lst_elm_t *lst_head;
    size_t max_num_elems;
    vvvv_evnt_lst_elm_t elems[1];
} vvvv_evnt_lst_t;

typedef struct vvvv_evnt_sq_t {
    /* The time between two adjacent indices in evnt_lsts */
    vvvv_tmstmp_t tick_dur; 
    /* Sequence length as number of elements in evnt_lists */
    size_t len; 
    vvvv_evnt_lst_t *evnt_lsts[1];
} vvvv_evnt_sq_t;

/* For use with stack allocation
 * Can also be used with malloc allocations, simply pass the pointer to the
 * allocated space to via v. The name of the allocated space should be v_data__.
 */
#define VVVV_EVNT_SEQ_INIT(v,sl,mne,td) \
    do { \
        assert(sl > 0); \
        assert(mne > 0); \
        char *memptr = v ## _data__; \
        v = (vvvv_evnt_sq_t*)memptr; \
        v->len = sl; \
        v->tick_dur = td; \
        memptr += sizeof(vvvv_evnt_sq_t) + sizeof(vvvv_evnt_lst_t*) * sl; \
        size_t n; \
        for (n = 0; n < sl; n++) { \
            v->evnt_lsts[n] = (vvvv_evnt_lst_t*)memptr; \
            v->evnt_lsts[n]->max_num_elems = mne; \
            v->evnt_lsts[n]->lst_head = NULL; \
            memptr += sizeof(vvvv_evnt_lst_t) + sizeof(vvvv_evnt_lst_elm_t) * mne; \
        } \
    } while (0)

/* Allocation without malloc.
 * t is the type.
 * For stack allocation, pass nothing for t. Note this might not be wise for
 * large sizes.
 * For static memory allocation (allocation at binary load) pass "static" for t.
 * Note that in the latter case, sl, and mne, the sequence length and maximum
 * number of elements respectively, must be determined at compile time and not
 * runtime. Your compiler will probably complain if it is not the case. Also
 * note in this case this is not really dynamic memory as the allocated memory
 * sticks around for the entire life of the program.
 */
#define VVVV_EVNT_SEQ_STACK_ALLOC(v,t,sl,mne,td) \
    t char v ## _data__[sizeof(vvvv_evnt_sq_t) + \
           (sizeof(vvvv_evnt_lst_t*) + \
                sizeof(vvvv_evnt_lst_t) + sizeof(vvvv_evnt_lst_elm_t) * mne) * sl]; \
    VVVV_EVNT_SEQ_INIT(v,sl,mne,td)

/* Callback information passed to vvvv_evnt_sq_map_range */
typedef struct vvvv_evnt_sq_mp_rng_t {
    void (*func)(vvvv_evnt_sq_t *, // The event sequence this was called on.
                 size_t, // The index of the event in the sequence.
                 vvvv_evnt_t *, // The event.
                 void *); // The auxiliary field in this structure.
    void *aux;
} vvvv_evnt_sq_mp_rng_t;

void vvvv_evnt_sq_map_range(vvvv_evnt_sq_t *seq,
                            vvvv_tmstmp_t tmin,
                            vvvv_tmstmp_t tmax,
                            vvvv_evnt_sq_mp_rng_t *cb);

vvvv_err_t vvvv_evnt_sq_add(vvvv_evnt_sq_t *seq, vvvv_evnt_t *evnt);

vvvv_err_t vvvv_evnt_lst_add(vvvv_evnt_lst_t *elt, vvvv_evnt_t *evnt);

typedef enum {
    vvvv_evnt_lst_rm_opt_NONE = 0,
    vvvv_evnt_lst_rm_opt_FREVNT = (1 << 0),
} vvvv_evnt_lst_rm_opt_t;

void vvvv_evnt_lst_rm_elem(vvvv_evnt_lst_t *lst,
                           vvvv_evnt_lst_elm_t *elm,
                           vvvv_evnt_lst_rm_opt_t opt);

#endif /* EVNT_SQ_H */
