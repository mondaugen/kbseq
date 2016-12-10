#ifndef EVNT_H
#define EVNT_H 

/*
evnt.h
Event base class
*/

#include "err.h" 
#include "midi.h" 
#include "tmstmp.h" 
#include "ord.h" 

#include <stddef.h> 
#include <stdint.h> 

typedef struct vvvv_evnt_t {
    vvvv_tmstmp_t ts;
    vvvv_tmstmp_t len;
    void (*free)(struct vvvv_evnt_t *);
    /* Returns value that can be used to sort events.
     * A possible use case for this is if the events represent notes with
     * pitches: this would allow you to sort the notes in some kind of order
     * based on their pitches.
     */
    vvvv_ord_t (*get_ord)(struct vvvv_evnt_t *);
    /*
    Function for getting MIDI data representation of event.
    the vvvv_evnt_t is this structure,
    the pointer to vvvv_midi_pckt_lst_t is the list in which to store the
    midi bytes
    size_t is the size of this list in bytes
    */
    vvvv_err_t (*get_midi_pckt_lst)(struct vvvv_evnt_t *, vvvv_midi_pckt_lst_t *, size_t);
} vvvv_evnt_t;

#define vvvv_evnt_set_get_midi_pckt_lst(ev,f) ((vvvv_evnt_t*)ev)->get_midi_pckt_lst = f 
#define vvvv_evnt_set_free(ev,f)              ((vvvv_evnt_t*)ev->free = f 

void vvvv_evnt_init(vvvv_evnt_t *ev,
                    vvvv_tmstmp_t ts,
                    vvvv_tmstmp_t len);

#endif /* EVNT_H */
