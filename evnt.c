#include "evnt.h" 


static vvvv_err_t vvvv_evnt_get_midi_pckt_lst(vvvv_evnt_t *ev,
                                    vvvv_midi_pckt_lst_t *pck,
                                    size_t len) VVVV_DONT_CALL;

static void vvvv_evnt_free(vvvv_evnt_t *ev) VVVV_DONT_CALL;

static vvvv_ord_t vvvv_evnt_get_ord(vvvv_evnt_t *ev) VVVV_DONT_CALL;

void vvvv_evnt_init(vvvv_evnt_t *ev,
                    vvvv_tmstmp_t ts,
                    vvvv_tmstmp_t len)
{
    ev->ts = ts;
    ev->len = len;
    ev->free = vvvv_evnt_free;
    ev->get_ord = vvvv_evnt_get_ord;
    ev->get_midi_pckt_lst = vvvv_evnt_get_midi_pckt_lst;
}

