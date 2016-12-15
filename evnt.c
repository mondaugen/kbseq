#include "evnt.h" 

static vvvv_evnt_vt_t vvvv_evnt_vt;
static vvvv_evnt_vt_t *vvvv_evnt_vt_p = NULL;

static vvvv_err_t evnt_get_midi_pckt_lst(vvvv_evnt_t *ev,
                                    vvvv_midi_pckt_lst_t *pck,
                                    size_t len) VVVV_DONT_CALL;

static void evnt_free(vvvv_evnt_t *ev) VVVV_DONT_CALL;

static vvvv_ord_t evnt_get_ord(vvvv_evnt_t *ev) VVVV_DONT_CALL;

vvvv_evnt_vt_t *vvvv_evnt_vt_init(vvvv_evnt_vt_t *vt)
{
    
    vt->get_ord = evnt_get_ord;
    vt->get_midi_pckt_lst = evnt_get_midi_pckt_lst;
    return vt;
}

void vvvv_evnt_init(vvvv_evnt_t *ev,
                    vvvv_tmstmp_t ts,
                    vvvv_tmstmp_t len)
{
    ev->ts = ts;
    ev->len = len;
    if (!vvvv_evnt_vt_p) {
        vvvv_evnt_vt_p = vvvv_evnt_vt_init(&vvvv_evnt_vt);
    }
    vvvv_evnt_set_v(ev,vvvv_evnt_vt_p);
    ev->free = evnt_free;
}

