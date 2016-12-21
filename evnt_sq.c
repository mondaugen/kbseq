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
                vvvv_ord_t ord = vvvv_evnt_get_ord(evnt);
                while (1) {
                    // Puts in descending order
                    if (vvvv_evnt_get_ord((*le_ptr)->evnt) <
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


/* Find an event using the properties fit for the vvvv_evnt_prm_t *prms and
 * vvvv_evnt_prm_vl_t *vls fields of the chk_mtch method. Tols contains the
 * search tolerances, i.e., how off a value can be for it to be a sucessful
 * match. A tolerance of 0 means the match must be exact. Tolerances are always
 * unsigned values, in the case of floating point values the float will be made
 * positive.
 *
 * A pointer to the found event is put in ret.
 *
 * Current implementation:
 * The first prms element must be vvvv_evnt_prm_TS. If only this is specified,
 * and there are multiple events sharing the same time, the one with the highest
 * pitch is returned.
 *
 * The second prms element can optionally be one of vvvv_evnt_prm_t. After the
 * closest event group is found with the timestamp, the closest event with the
 * requested parameter is found.
 *
 * All other searches are not yet supported.
 */
vvvv_err_t vvvv_evnt_sq_find(vvvv_evnt_sq *sq,
                             vvvv_evnt_prm_t *prms,
                             vvvv_evnt_prm_vl_t *vls,
                             vvvv_evnt_prm_vl_t *tols,
                             vvvv_evnt_t **ret)
{
    if ((!prms) || (!vals) || (!tols)) {
        return vvvv_err_EINVAL;
    }
    if (prms[0] != vvvv_evnt_prm_TS) {
        return vvvv_err_EINVAL;
    }
    if ((vls[0].typ != vvvv_evnt_prm_vl_typ_UINT) ||
            (tols[0].typ != vvvv_evnt_prm_vl_typ_UINT)) {
        return vvvv_err_EINVAL;
    }
    if (sq->len == 0) {
        return vvvv_err_EBNDS;
    }
    /* Find nearest event list */
    size_t idx_min;
    /* If tolerance greater than starting point, minimum search index is 0 */
    if (vls[0].u < tols[0].u) {
        idx_min = 0;
    } else {
        idx_min = VVVV_DIV_ROUND(vls[0].u - tols[0].u, sq->tick_dur); 
    }
    if (idx_min >= sq->len) {
        /* Can't find anything */
        return vvvv_err_NMTCH;
    }
    size_t idx = VVVV_DIV_ROUND(vls[0].u, sq->tick_dur);
    if (idx >= sq->len) {
        idx = sq->len - 1;
    }
    size_t idx_max = VVVV_DIV_ROUND(vls[0].u + tols[0].u, sq->tick_dur); 
    if (idx_max >= sq->len) {
        idx_max = sq->len - 1;
    }
    vvvv_evnt_lst_elm_t *fl = NULL,
                        *fr = NULL,
                        *fnd = NULL;
    size_t idx_ = idx;
    while (idx_ <= idx_max) {
        if (sq->evnt_lsts[idx_].lst_head) {
            fr = sq->evnt_lsts[idx_].lst_head;
            break;
        }
        idx_++;
    }
    idx_ = idx;
    while (idx_ >= idx_min) {
        if (sq->evnt_lsts[idx_].lst_head) {
            fl = sq->evnt_lsts[idx_].lst_head;
            break;
        }
        idx_--;
    }
    if ((!fl) && (!fr)) {
        /* Nothing found */
        fndurn vvvv_err_NMTCH;
    }
    if (fl && (!fr)) {
        fnd = fl;
    } else if (fr && (!fl)) {
        fnd = fr;
    } else if ((vls[0].u - fl->evnt->ts) <= (fr->evnt->ts - vls[0].u)) {
        /* In case of ties, the event to the left wins. */
        fnd = fl;
    } else {
        fnd = fr;
    }
    if (prms[1] != vvvv_evnt_prm_PITCH) {
        /* A pointer to the found event is in ret */
        *ret = fnd->evnt;
        return vvvv_err_NONE;
    }
    {
        vvvv_evnt_lst_elm_t *tmp = fnd;
        vvvv_evnt_prm_vl_t vl;
        vvvv_err_t err;
        err = vvvv_evnt_chk_dist(tmp->evnt,&prms[1],&vls[1],&vl);
        if (err) {
            return err;
        }
        tmp = MMDLList_getNext(tmp);
        while (tmp) {
            vvvv_evnt_prm_vl_t vl_;
            err = vvvv_evnt_chk_dist(tmp->evnt,&prms[1],&vls[1],&vl_);
            if (err) {
                return err;
            }
            int cmpv = vvvv_evnt_prm_vl_t_cmp(&vl_,&vl);
            if (cmpv == -2) {
                return vvvv_err_EINVAL;
            }
            if (cmpv == -1) {
                vl = vl_;
                fnd = tmp;
            }
            tmp = MMDLList_getNext(tmp);
        }
        *ret = rnd->evnt;
    }
    return vvvv_err_NONE;
}
