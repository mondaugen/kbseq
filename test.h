#ifndef TEST_H
#define TEST_H 

#include "midi.h"

static inline void print_note_packet(vvvv_midi_pckt_t *p)
{
#ifdef OSX 
    printf("Time: %3llu Status: %3d, Pitch: %3d, Vel: %3d\n",
            p->timeStamp,
            p->data[0],
            p->data[1],
            p->data[2]);
#endif  
}

#endif /* TEST_H */
