#ifndef EVNT_H
#define EVNT_H 

/*
evnt.h
Event base class
*/

#include "midi.h" 

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
    void get_midi_pckt_lst(vvvv_evnt_t *, vvvv_midi_pckt_lst_t *, size_t);
} vvvv_evnt_t;

void vvvv_evnt_init(vvvv_evnt_t *ev,
                    vvvv_tmstmp_t ts,
                    vvvv_tmstmp_t len,
                    void get_midi_pckt_lst(vvvv_evnt_t *, vvvv_midi_pckt_lst_t *, size_t));

typedef enum {
    vvvv_nt_evnt_typ_UNKNOWN,
    vvvv_nt_evnt_typ_PITCHED,
} vvvv_nt_evnt_typ_t;

typedef struct vvvv_nt_evnt_ptchd_t {
    float pitch;
    float vel;
} vvvv_nt_evnt_ptchd_t;

typedef union {
    vvvv_nt_evnt_ptchd_t pitched;
} vvvv_nt_evnt_dt_t;

typedef struct vvvv_nt_evnt_t {
    vvvv_evnt_t super;
    vvvv_nt_evnt_typ_t typ;
    vvvv_nt_evnt_dt_t dt;
} vvvv_nt_evnt_t;

void vvvv_nt_evnt_init(vvvv_nt_evnt_t *ev,
                       vvvv_tmstmp_t ts,
                       vvvv_tmstmp_t len,
                       vvvv_nt_evnt_typ_t type);


#endif /* EVNT_H */
