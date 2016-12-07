#ifndef EVNT_H
#define EVNT_H 

/*
evnt.h
Event base class
*/

#include "err.h" 
#include "midi.h" 
#include "tmstmp.h" 

#include <stddef.h> 
#include <stdint.h> 

typedef struct vvvv_evnt_t {
    vvvv_tmstmp_t ts;
    vvvv_tmstmp_t len;
    /*
    Function for getting MIDI data representation of event.
    the vvvv_evnt_t is this structure,
    the pointer to vvvv_midi_pckt_lst_t is the list in which to store the
    midi bytes
    size_t is the size of this list in bytes
    */
    vvvv_err_t (*get_midi_pckt_lst)(struct vvvv_evnt_t *, vvvv_midi_pckt_lst_t *, size_t);
} vvvv_evnt_t;

void vvvv_evnt_init(vvvv_evnt_t *ev,
                    vvvv_tmstmp_t ts,
                    vvvv_tmstmp_t len,
                    vvvv_err_t get_midi_pckt_lst(vvvv_evnt_t *, vvvv_midi_pckt_lst_t *, size_t));

#endif /* EVNT_H */
