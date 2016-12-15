#include "nt_evnt.h"
#include "midi.h" 

#if HAVE_MALLOC
#include <stdlib.h>
#endif

static vvvv_nt_evnt_vt_t  vvvv_nt_evnt_vt;
static vvvv_nt_evnt_vt_t *vvvv_nt_evnt_vt_p = NULL;

/*
Pitch simply casted,
Velocity assumed to be between 0 and 1, and is scaled appropriately.
*/
static vvvv_err_t get_midi_note_on(vvvv_nt_evnt_t *nev, vvvv_midi_pckt_t *pckt)
{
#ifdef OSX
    pckt->timeStamp = ((vvvv_evnt_t*)nev)->ts;
    pckt->length = 3;
    /* Done this way to preserve channel if present */
    pckt->data[0] &= 0xf0;
    pckt->data[0] |= MIDIMSG_NOTE_ON;
    pckt->data[1] = ((unsigned char)nev->dt.pitched.pitch) % MIDIMSG_DATA_BYTE_MAX;
    pckt->data[2] = (unsigned char)(nev->dt.pitched.vel * MIDIMSG_DATA_BYTE_MAX);
    return vvvv_err_NONE;
#else
    return vvvv_err_NOTIMP;
#endif
} 

/*
Pitch simply casted,
Velocity just set as 0 (even though it is technically possible to have non-zero note off velocities).
*/
static vvvv_err_t get_midi_note_off(vvvv_nt_evnt_t *nev, vvvv_midi_pckt_t *pckt)
{
#ifdef OSX
    pckt->timeStamp = ((vvvv_evnt_t*)nev)->ts + ((vvvv_evnt_t*)nev)->len;
    pckt->length = 3;
    /* Done this way to preserve channel if present */
    pckt->data[0] &= 0xf0;
    pckt->data[0] |= MIDIMSG_NOTE_OFF;
    pckt->data[1] = ((unsigned char)nev->dt.pitched.pitch) % MIDIMSG_DATA_BYTE_MAX;
    pckt->data[2] = 0;
    return vvvv_err_NONE;
#else
    return vvvv_err_NOTIMP;
#endif
} 

static vvvv_err_t get_midi_pckt_lst(vvvv_evnt_t *ev,
                                    vvvv_midi_pckt_lst_t *lst,
                                    size_t sz)
{
    vvvv_nt_evnt_t *nev = (vvvv_nt_evnt_t*)ev;
    switch (nev->typ) {
        case vvvv_nt_evnt_typ_PITCHED:
            if (sz < VVVV_MIDI_PCKT_LST_SZ(2)) {
                return vvvv_err_EINVAL;
            }
#ifdef OSX
            lst->numPackets = 2;
            vvvv_midi_pckt_t *pckt = &lst->packet[0];
            (void) nev->dt.pitched.get_midi_note_on(nev,pckt);
            pckt = MIDIPacketNext(pckt);
            (void) nev->dt.pitched.get_midi_note_off(nev,pckt);
            return vvvv_err_NONE;
#else
            return vvvv_err_NOTIMP;
#endif
        case vvvv_nt_evnt_typ_UNKNOWN:
        default:
            return vvvv_err_EINVAL;
    }
}

static vvvv_ord_t get_ord(vvvv_evnt_t *ev)
{
    vvvv_nt_evnt_t *nev = (vvvv_nt_evnt_t*)ev;
    if (nev->typ == vvvv_nt_evnt_typ_PITCHED) {
        return (vvvv_ord_t)nev->dt.pitched.pitch;
    }
    return 0;
}

vvvv_nt_evnt_vt_t *vvvv_nt_evnt_vt_init(vvvv_nt_evnt_vt_t *vt)
{
    vt = (vvvv_nt_evnt_vt_t*)vvvv_evnt_vt_init((vvvv_evnt_vt_t*)vt);
    ((vvvv_evnt_vt_t*)vt)->get_ord = get_ord;
    ((vvvv_evnt_vt_t*)vt)->get_midi_pckt_lst = get_midi_pckt_lst;
    return vt;
}

void vvvv_nt_evnt_init(vvvv_nt_evnt_t *nev,
                       vvvv_tmstmp_t ts,
                       vvvv_tmstmp_t len,
                       vvvv_nt_evnt_typ_t typ)
{
#ifdef DEBUG 
    memset(nev,0,sizeof(vvvv_nt_evnt_t));
#endif  
    vvvv_evnt_init((vvvv_evnt_t*)nev, ts, len);
    if (!vvvv_nt_evnt_vt_p) {
        vvvv_nt_evnt_vt_p = vvvv_nt_evnt_vt_init(&vvvv_nt_evnt_vt);
    }
    vvvv_evnt_set_v(nev,vvvv_nt_evnt_vt_p);
    switch (typ) {
        case vvvv_nt_evnt_typ_PITCHED:
            nev->typ = typ;
            nev->dt.pitched.get_midi_note_on = get_midi_note_on;
            nev->dt.pitched.get_midi_note_off = get_midi_note_off;
            break;
        // Otherwise not initialized
        case vvvv_nt_evnt_typ_UNKNOWN:
        default:
            break;
    }
}

static void nt_evnt_free(vvvv_evnt_t* ev)
{
#if HAVE_MALLOC
    free(ev);
#else
#error "No allocation method defined."
#endif
}

/* Allocate pointer to new initialized nt_evnt.
 * Returns NULL if allocation fails, e.g., if out of memory.
 */
vvvv_nt_evnt_t *vvvv_nt_evnt_new(vvvv_tmstmp_t ts,
                                 vvvv_tmstmp_t len,
                                 vvvv_nt_evnt_typ_t typ)
{
    vvvv_nt_evnt_t *ret;
#if HAVE_MALLOC
    ret = (vvvv_nt_evnt_t*)malloc(sizeof(vvvv_nt_evnt_t));
    if (!ret) {
        return NULL;
    }
#else
#error "No allocation method defined."
#endif
    vvvv_nt_evnt_init(ret,ts,len,typ);
    vvvv_evnt_set_free(ret,nt_evnt_free);
    return ret;
}
