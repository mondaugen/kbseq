#include "evnt.h" 

static vvvv_evnt_vt_t vvvv_evnt_vt;
static vvvv_evnt_vt_t *vvvv_evnt_vt_p = NULL;

static vvvv_err_t evnt_get_midi_pckt_lst(vvvv_evnt_t *ev,
                                    vvvv_midi_pckt_lst_t *pck,
                                    size_t len) VVVV_DONT_CALL;

static void evnt_free(vvvv_evnt_t *ev) VVVV_DONT_CALL;

static vvvv_ord_t evnt_get_ord(vvvv_evnt_t *ev) VVVV_DONT_CALL;

/* This event public because subclasses may still want to call it.
 * The default way for events to check for matches. 
 * If vvvv_evnt_prm_DUR is requested, and the value equals the dur field, then
 * vvvv_err_NONE is returned so long there is no other vvvv_evnt_prm_DUR
 * requested whose value does not match.  All other parameters requested are
 * ignored, so if vvvv_evnt_prm_DUR isn't even requested, it will also return
 * vvvv_err_NONE.  If the value is not the value requested, it will of course
 * return vvvv_err_NMTCH.  If the value is not of unsigned int, vvvv_err_EINVAL
 * is returned.
 */
vvvv_err_t vvvv_evnt_chk_mtch_dflt(struct vvvv_evnt_t *ev,
                              vvvv_evnt_prm_t *prms,
                              vvvv_evnt_prm_vl_t *vls)
{
    if ((!prms) || (!vls)) {
        return vvvv_err_EINVAL;
    }
    int matched = 1;
    while (matched && (*prms != vvvv_evnt_prm_END)) {
        switch (*prms) {
            case vvvv_evnt_prm_TS:
                if (vls->typ != vvvv_evnt_prm_vl_typ_UINT) {
                    return vvvv_err_EINVAL;
                }
                matched &= (vls->u == ev->ts);
                break;
            case vvvv_evnt_prm_DUR:
                if (vls->typ != vvvv_evnt_prm_vl_typ_UINT) {
                    return vvvv_err_EINVAL;
                }
                matched &= (vls->u == ev->dur);
                break;
            default:
                break;
        }
        prms++;
        vls++;
    }
    if (!matched) {
        return vvvv_err_NMTCH;
    }
    return vvvv_err_NONE;
}

/* Check the distance of the event's parameter from a value. Returns
 * vvvv_err_EINVAL if the parameter is not supported by this event.
 * If no error, the result is placed in ret and vvvv_err_NONE is returned. 
 *
 * Currently prm and vl will only have their first entries read if they are
 * arrays.
 */
vvvv_err_t vvvv_evnt_chk_dist_dflt(vvvv_evnt_t *ev,
                                   vvvv_evnt_prm_t *prm,
                                   vvvv_evnt_prm_vl_t *vl,
                                   vvvv_evnt_prm_vl_t *ret)
{
    if ((!prm) || (*vl) || (*ret)) {
        return vvvv_err_EINVAL;
    }
    switch (*prm) {
        case vvvv_evnt_prm_TS:
            if (vl->typ != vvvv_evnt_prm_vl_typ_UINT) {
                return vvvv_err_EINVAL;
            }
            ret->typ = vvvv_evnt_prm_vl_typ_UINT;
            ret->u = VVVV_NORM_L1(ev->ts,vl->u);
            return vvvv_err_NONE;
        default:
            return vvvv_err_EINVAL;
    }
}

vvvv_evnt_vt_t *vvvv_evnt_vt_init(vvvv_evnt_vt_t *vt)
{
    
    vt->get_ord = evnt_get_ord;
    vt->get_midi_pckt_lst = evnt_get_midi_pckt_lst;
    vt->chk_mtch = vvvv_evnt_chk_mtch_dflt;
    vt->chk_dist = vvvv_evnt_chk_dist_dflt;
    return vt;
}

void vvvv_evnt_init(vvvv_evnt_t *ev,
                    vvvv_tmstmp_t ts,
                    vvvv_tmstmp_t dur)
{
    ev->ts = ts;
    ev->dur = dur;
    if (!vvvv_evnt_vt_p) {
        vvvv_evnt_vt_p = vvvv_evnt_vt_init(&vvvv_evnt_vt);
    }
    vvvv_evnt_set_v(ev,vvvv_evnt_vt_p);
    ev->free = evnt_free;
}

