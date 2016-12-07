#include "evnt.h" 

void vvvv_evnt_init(vvvv_evnt_t *ev,
                    vvvv_tmstmp_t ts,
                    vvvv_tmstmp_t len,
                    vvvv_err_t get_midi_pckt_lst(vvvv_evnt_t *, vvvv_midi_pckt_lst_t *, size_t))
{
    ev->ts = ts;
    ev->len = len;
    ev->get_midi_pckt_lst = get_midi_pckt_lst;
}

