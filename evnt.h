#ifndef EVNT_H
#define EVNT_H 

/*
evnt.h
Event abstract class
*/

#include "midi.h" 

typedef struct vvvv_evnt_t {
    void get_midi_pckt_lst(vvvv_evnt_t *, vvvv_midi_pckt_lst_t **);
} vvvv_evnt_t;

#endif /* EVNT_H */
