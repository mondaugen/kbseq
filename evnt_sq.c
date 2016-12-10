/*
 * evnt_sq.c
 *
 * Stores a sequence of events.
 */

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
#define __VVVV_EVNT_SEQ_INIT(v,sl,mne) \
    do { \
#ifdef DEBUG \
        assert(sl > 0); \
        assert(mne > 0); \
#endif \
        char *memptr = v ## _data__; \
        v = (vvvv_evnt_sq_t*)memptr; \
        v->len = sl; \
        memptr += sizeof(vvvv_evnt_sq_t) + sizeof(vvvv_evnt_lst_t*) * sl; \
        size_t n; \
        for (n = 0; n < sl; n++) { \
            v->lists[n] = (vvvv_evnt_lst_t*)memptr; \
            v->lists[n]->max_num_elems = mne; \
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
#define VVVV_EVNT_SEQ_STACK_ALLOC(var,t,sl,mne)\
    t char v ## _data__[sizeof(vvvv_evnt_sq_t) + \
           (sizeof(vvvv_evnt_lst_t*) + \
                sizeof(vvvv_evnt_lst_t) + sizeof(vvvv_evnt_lst_elm_t) * mne) * sl]; \
    __VVVV_EVNT_SEQ_INIT(v,sl,mne)

/* Callback information passed to vvvv_evnt_sq_map_range */
typedef struct vvvv_evnt_sq_mp_rng_t {
    void (*func)(vvvv_evnt_sq_t *, // The event sequence this was called on.
                 size_t, // The index of the event in the sequence.
                 vvvv_evnt_t *, // The event.
                 void *); // The auxiliary field in this structure.
    void *aux;
} vvvv_evnt_sq_mp_rng_t;

/* Call function on all items between tmin and tmax where these times are
 * specified as timestamps.
 */
void vvvv_evnt_sq_map_range(vvvv_evnt_sq_t *seq,
                            vvvv_tmstmp_t tmin,
                            vvvv_tmstmp_t tmax,
                            vvvv_evnt_sq_mp_rng_t *cb)
{
    size_t idx = VVVV_DIV_CEIL(tmin/seq->tick_dur);
    while ((idx < (seq->tick_dur * seq->len)) &&
            ((idx * seq->tick_dur) < tmax)) {
        vvvv_evnt_lst_elm_t *lst = seq->evnt_lsts[idx].head;
        while (lst) {
            cb->func(seq, idx, lst->evnt, cb->aux);
            lst = (vvvv_evnt_lst_elm_t*)MMDLList_getNext(lst);
        }
        idx++;
    }
}

vvvv_err_t vvvv_evnt_lst_add(vvvv_evnt_lst_t *elt, vvvv_evnt_t *evnt)
{
    size_t n;
    for (n = 0; n < elt->max_num_elems; n++) {
        if (elt->elems[n].evnt == NULL) {
            elt->elems[n].evnt = evnt;
            if (!elt->elems[n].lst_head) {
                elt->elems[n].lst_head = &elt->elems[n];
            } else {
                vvvv_evnt_lst_elm_t **le_ptr = &elt->elems[n].lst_head;
                vvvv_ord_t ord = evnt->get_ord(evnt);
                while (1) {
                    // Puts in descending order
                    if ((*le_ptr)->evnt->get_ord((*le_ptr)->evnt) <
                            ord) {
                        (void) MMDLList_insertBefore(*le_ptr,&elt->elems[n]);
                        *le_ptr = &elt->elems[n];
                        break;
                    }
                    if ((*le_ptr)->next) {
                        le_ptr = &MMDLList_getNext((*le_ptr));
                    } else {
                        (void) MMDLList_insertAfter((*le_ptr),&elt->elems[n]);
                        break;
                    }
                }
            }
            // Event found a free spot
            return vvvv_err_NONE;
        }
    }
    // No room for event
    return vvvv_err_EFULL;
}

typedef enum {
    vvvv_evnt_lst_rm_opt_NONE = 0,
    vvvv_evnt_lst_rm_opt_FREVNT = (1 << 0),
} vvvv_evnt_lst_rm_opt_t;

/* Removes element from list.
 * If item is list head, the list head is replaced by the element following elm.
 */
void vvvv_evnt_lst_rm_elem(vvvv_evnt_lst_t *lst,
                           vvvv_evnt_lst_elm_t *elm,
                           vvvv_evnt_lst_rm_opt_t opt)
{
    vvvv_evnt_lst_elm_t *next = (vvvv_evnt_lst_elm_t*)MMDLList_getNext(elm);
    MMDLList_remove((MMDLList*)elm);
    ((MMDLList*)elm)->next = NULL;
    ((MMDLList*)elm)->prev = NULL;
    if (lst->list_head == elm) {
        lst->list_head = next;
    }
    if (opt & vvvv_evnt_lst_rm_opt_FREVNT) {
        elm->evnt->free(elm->evnt);
    }
}
