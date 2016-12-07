/* mnoh_test.c
 *
 * Test the MIDI note off heap.
 */
#include "mnoh.h"
#include "test.h" 

	//MIDITimeStamp		timeStamp;
	//UInt16				length;
	//Byte				data[256];

#define MAX_TIME 23

int vvvv_mnoh_test_ordered(void)
{
    vvvv_midi_pckt_t noteon_packets[] = {
        {0,3,{MIDIMSG_NOTE_ON,1,100}},
        {2,3,{MIDIMSG_NOTE_ON,2,99}},
        {4,3,{MIDIMSG_NOTE_ON,3,98}},
        {6,3,{MIDIMSG_NOTE_ON,4,97}},
        {8,3,{MIDIMSG_NOTE_ON,5,96}},
    };

    vvvv_midi_pckt_t noteoff_packets[] = {
        {14,3,{MIDIMSG_NOTE_OFF,1,0}},
        {16,3,{MIDIMSG_NOTE_OFF,2,100}},
        {18,3,{MIDIMSG_NOTE_OFF,3,0}},
        {20,3,{MIDIMSG_NOTE_OFF,4,0}},
        {22,3,{MIDIMSG_NOTE_OFF,5,0}},
        {19,3,{MIDIMSG_NOTE_OFF,2,11}},
        {17,3,{MIDIMSG_NOTE_OFF,3,0}},
        {21,3,{MIDIMSG_NOTE_OFF,1,11}}
    };

    size_t i;
    vvvv_mnoh_t *mnoh;
    VVVV_MNOH_ALLOC_STACK(mnoh);
    vvvv_mnoh_init(mnoh);
    for (i = 0;
         i < (sizeof(noteoff_packets)/sizeof(vvvv_midi_pckt_t));
         i++) {
        vvvv_mnoh_insert(mnoh, &noteoff_packets[i]);
    }

#ifdef OSX
    MIDITimeStamp time = 0;
#endif 
    while (time < MAX_TIME) {
        for (i = 0;
             i < (sizeof(noteon_packets)/sizeof(vvvv_midi_pckt_t));
             i++) {
            if (time == noteon_packets[i].timeStamp) {
                print_note_packet(&noteon_packets[i]);
            }
        }
        while (mnoh->heap.size) {
            vvvv_mnoh_elem_t *elem;
            elem = (vvvv_mnoh_elem_t*)(mnoh->heap.A[0]);
            if (elem->packet.timeStamp == time) {
                HeapErr err;
                err = Heap_pop(&mnoh->heap,(void**)&elem);
                if (!err) {
                    print_note_packet(&elem->packet);
                }
            } else {
                break;
            }
        }
        time++;
    }
    return 1;
}

int main (void)
{
    int passed = 1;
    passed &= vvvv_mnoh_test_ordered();
    return -1 * (!passed);
}

