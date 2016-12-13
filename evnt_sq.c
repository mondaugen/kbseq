/*
 * evnt_sq.c
 *
 * Stores a sequence of events.
 */

#include "evnt_sq.h" 

/* Call function on all items between tmin and tmax where these times are
 * specified as timestamps.
 */
void vvvv_evnt_sq_map_range(vvvv_evnt_sq_t *seq,
                            vvvv_tmstmp_t tmin,
                            vvvv_tmstmp_t tmax,
                            vvvv_evnt_sq_mp_rng_t *cb)
{
    size_t idx = VVVV_DIV_CEIL(tmin,seq->tick_dur);
    while ((idx < (seq->tick_dur * seq->len)) &&
            ((idx * seq->tick_dur) < tmax)) {
        vvvv_evnt_lst_elm_t *lst = seq->evnt_lsts[idx]->lst_head;
        while (lst) {
            cb->func(seq, idx, lst->evnt, cb->aux);
            lst = (vvvv_evnt_lst_elm_t*)MMDLList_getNext(lst);
        }
        idx++;
    }
}

/* Adds event to sequence at index round(evnt->ts/seq->tick_dur) */
vvvv_err_t vvvv_evnt_sq_add(vvvv_evnt_sq_t *seq, vvvv_evnt_t *evnt)
{
    size_t idx = VVVV_DIV_ROUND(evnt->ts, seq->tick_dur);
    if (idx < seq->len) {
        return vvvv_evnt_lst_add(seq->evnt_lsts[idx],
                (vvvv_evnt_t*)evnt);
    }
    return vvvv_err_EBNDS;
}

vvvv_err_t vvvv_evnt_lst_add(vvvv_evnt_lst_t *elt, vvvv_evnt_t *evnt)
{
    size_t n;
    for (n = 0; n < elt->max_num_elems; n++) {
        if (elt->elems[n].evnt == NULL) {
            elt->elems[n].evnt = evnt;
            if (!elt->lst_head) {
                elt->lst_head = &elt->elems[n];
            } else {
                vvvv_evnt_lst_elm_t **le_ptr = &elt->lst_head;
                vvvv_ord_t ord = evnt->get_ord(evnt);
                while (1) {
                    // Puts in descending order
                    if ((*le_ptr)->evnt->get_ord((*le_ptr)->evnt) <
                            ord) {
                        (void) MMDLList_insertBefore((MMDLList*)*le_ptr,
                                (MMDLList*)&elt->elems[n]);
                        *le_ptr = &elt->elems[n];
                        break;
                    }
                    if (MMDLList_getNext(*le_ptr)) {
                        *le_ptr = (vvvv_evnt_lst_elm_t*)&MMDLList_getNext(*le_ptr);
                    } else {
                        (void) MMDLList_insertAfter((MMDLList*)*le_ptr,
                                (MMDLList*)&elt->elems[n]);
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
    if (lst->lst_head == elm) {
        lst->lst_head = next;
    }
    if (opt & vvvv_evnt_lst_rm_opt_FREVNT) {
        elm->evnt->free(elm->evnt);
    }
}
