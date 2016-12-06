#ifndef EVNT_H
#define EVNT_H 

/*
evnt.h
Event abstract class
*/

#include "midi.h" 

typedef struct vvvv_evnt_t {
    void get_midipcktlst(vvvv_evnt_t *, vvvv_midipcktlst_t **);
} vvvv_evnt_t;

#endif /* EVNT_H */
