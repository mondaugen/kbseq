#include "sq.h"
#include <stdlib.h> 

/* Adds event to sequence at index round(evnt->ts/seq->tick_dur) */
vvvv_err_t vvvv_sq_add(vvvv_sq_t *seq, vvvv_sq_lst_elm_t *sle)
{
    size_t idx = VVVV_DIV_ROUND(sle->vt->get_ts(se->dat), seq->tick_dur);
    if (idx < seq->len) {
        return vvvv_sq_lst_add(seq->sq_elms[idx], sle);
    }
    return vvvv_err_EBNDS;
}

/* For sorting in ascending order. */
int vvvv_sq_lst_elm_cmp_desc(void *a_, void *b_)
{
    vvvv_sq_lst_elm_t *a = *((vvvv_sq_lst_elm_t**)a_);
    vvvv_sq_lst_elm_t *b = *((vvvv_sq_lst_elm_t**)b_);
    return (a->vt->get_ord(a) > b->vt->get_ord(b))
        - (a->vt->get_ord(a) < b->vt->get_ord(b));
}

/* Sorts the list after this item. Leaves this item where it is but the list
 * after it is sorted in descending order. */
vvvv_err_t vvvv_sq_elm_lst_sort(vvvv_sq_lst_elm_t *sle)
{
    if (!sle) {
        return vvvv_err_EINVAL;
    }
    /* Just use qsort for now. */
    size_t l_len = MMDLList_getLength(MMDLList_getNext(sle));
    if (l_len == 0) {
        return vvvv_err_EINVAL;
    }
    vvvv_sq_lst_elm_t *elms[l_len];
    vvvv_sq_lst_elm_t *tmp;
    size_t i;
    tmp = (vvvv_sq_lst_elm_t*)MMDLList_getNext(sle);
    while (tmp) {
        elms[i++] = tmp;
        tmp = MMDLList_getNext(tmp);
    }
    qsort((void*)elms,l_len,sizeof(vvvv_sq_lst_elm_t*),vvvv_sq_lst_elm_cmp_desc);
    MMDLList_setNext(sle,NULL);
    for (i = 0; i < l_len; i++) {
        MMDLList_insertAfter(sle,elms[i]);
    }
    return vvvv_err_NONE;
}

/* Inserts the item at the right place to maintain the correct ordering */
vvvv_err_t vvvv_sq_lst_elm_insert_ordered(vvvv_sq_lst_elm_t **le_ptr, vvvv_sq_lst_elm_t *sle)
{
    if ((!*le_ptr) || (!sle)) {
        return vvvv_err_EINVAL;
    }
    vvvv_ord_t ord = sle->vt->get_ord(sle->dat);
    while (1) {
        // Puts in descending order
        if ((*le_ptr)->vt->get_ord((*le_ptr)->dat) <
                ord) {
            (void) MMDLList_insertBefore((MMDLList*)*le_ptr,
                    (MMDLList*)sle);
            *le_ptr = sle;
            break;
        }
        if (MMDLList_getNext(*le_ptr)) {
            *le_ptr = (vvvv_evnt_lst_elm_t*)&MMDLList_getNext(*le_ptr);
        } else {
            (void) MMDLList_insertAfter((MMDLList*)*le_ptr,
                    (MMDLList*)sle);
            break;
        }
    }
    return vvvv_err_NONE;
}

vvvv_err_t vvvv_sq_elm_add(vvvv_sq_elm_t *se, vvvv_sq_lst_elm_t *sle)
{
    size_t n;
    vvvv_err_t err;
    if ((err = vvvv_sq_lst_elm_insert_ordered(&MMDLList_getNext(se->lst_head), sle))) {
        return err;
    }
    return vvvv_err_NONE;
}

/* Call function on all items between tmin and tmax where these times are
 * specified as timestamps.
 *  
 *  WARNING: Make sure you know what things are in the sq. You could be calling
 *  a function on anything!
 */
void vvvv_sq_map_range(vvvv_sq_t *seq,
                       vvvv_tmstmp_t tmin,
                       vvvv_tmstmp_t tmax,
                       vvvv_sq_mp_rng_t *cb)
{
    size_t idx = VVVV_DIV_CEIL(tmin,seq->tick_dur);
    while ((idx < (seq->tick_dur * seq->len)) &&
            ((idx * seq->tick_dur) < tmax)) {
        vvvv_evnt_lst_elm_t *lst = MMDLList_getNext(seq->evnt_lsts[idx]->lst_head);
        while (lst) {
            cb->func(seq, idx, lst->dat, cb->aux);
            lst = (vvvv_evnt_lst_elm_t*)MMDLList_getNext(lst);
        }
        idx++;
    }
}
