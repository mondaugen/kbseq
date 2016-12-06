/* mnoh.c
 *
 * MIDI note off heap.
 */

#include "mnoh.h" 

int vvvv_elem_cmp(void *a, void*b)
{
    vvvv_mnoh_elem_t *a_, *b_;
    a_ = (vvvv_mnoh_elem_t*)a;
    b_ = (vvvv_mnoh_elem_t*)b;
#ifdef OSX 
    return (a_->packet.timeStamp >= b_->packet.timeStamp);
#endif  
}

void vvvv_elem_set_idx(void *a, size_t idx)
{
    vvvv_mnoh_elem_t *a_;
    a_ = (vvvv_mnoh_elem_t*)a;
    a_->curidx = idx;
}

void vvvv_mnoh_init(vvvv_mnoh_t *mnoh)
{
    size_t i;
    /* No elements in heap initially */
    for (i = 0; i < VVVV_MNOH_ARRAY_SIZE; i++) {
        mnoh->elems[i].curidx = VVVV_MNOH_ARRAY_SIZE;
    }
    Heap_init(&mnoh->heap, VVVV_MNOH_ARRAY_SIZE, vvvv_elem_cmp, vvvv_elem_set_idx);
}

/* If passed a MIDIPacket representing a note off 
 * add it to the heap if its timestamp is further into the future than the one
 * on the heap, or a note off with this pitch is not present on the heap.
 */
vvvv_err_t vvvv_mnoh_insert(vvvv_mnoh_t *mnoh, vvvv_midi_pckt_t *p)
{
    if (!MIDIMSG_IS_NOTE_OFF(VVVV_MIDI_PCKT_GET_MSG_DATA(p)[0])) {
        return vvvv_err_EINVAL;
    }
    size_t pitch = (size_t)VVVV_MIDI_PCKT_NOTE_GET_PCH(p);
    if (mnoh->elems[pitch].curidx >= mnoh->heap.size) {
        /* Not in heap */
        mnoh->elems[pitch].packet = *p;
        (void) Heap_push(&mnoh->heap, (void*)&mnoh->elems[pitch]);
    } else if (VVVV_MIDI_PCKT_GET_TIMESTAMP(&mnoh->elems[pitch].packet) <
            VVVV_MIDI_PCKT_GET_TIMESTAMP(p)) {
        /* In heap but timestamp is less than this event's time stamp */
        mnoh->elems[pitch].packet = *p;
        Heap_heapify(&mnoh->heap, mnoh->elems[pitch].curidx);
    }
    /* Otherwise ignored, not really an error. */
    return vvvv_err_NONE;
}
