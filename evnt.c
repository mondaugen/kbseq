#include "evnt.h" 

void vvvv_evnt_init(vvvv_evnt_t *ev,
                    vvvv_tmstmp_t ts,
                    vvvv_tmstmp_t len,
                    void get_midi_pckt_lst(vvvv_evnt_t *, vvvv_midi_pckt_lst_t *, size_t))
{
    ev->ts = ts;
    ev->len = len;
    ev->get_midi_pckt_lst = get_midi_pckt_lst;
}

static vvvv_nt_evnt_get_midi_pckt_lst(vvvv_evnt_t *ev, vvvv_midi_pckt_lst_t *lst, size_t sz)
{
    vvvv_nt_evnt_t *nev = (vvvv_nt_evnt_t*)ev;
    switch (nev->typ) {
        case vvvv_nt_evnt_typ_PITCHED:


void vvvv_nt_evnt_init(vvvv_nt_evnt_t *ev,
                       vvvv_tmstmp_t ts,
                       vvvv_tmstmp_t len,
                       vvvv_nt_evnt_typ_t typ)
{
    vvvv_evnt_init((vvvv_evnt_t*)ev,
