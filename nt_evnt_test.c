#include "mnoh.h" 
#include "nt_evnt.h"
#include "test.h" 

#include <string.h> 

#if HAVE_MALLOC
#include <stdlib.h>
#endif

typedef struct note_info_t {
    vvvv_tmstmp_t ts;
    vvvv_tmstmp_t len;
    float pitch;
    float vel;
} note_info_t;

int vvvv_nt_evnt_no_overlaps_test(void)
{
    int passed = 1;
    static note_info_t notes[] = {
        {0,14,1,100./127.},
        {2,14,2,99./127.},
        {4,14,3,98./127.},
        {6,14,4,97./127.},
        {8,14,5,96./127.}
    };
#ifdef OSX
    static vvvv_midi_pckt_t correct_pckts[] = {
        {0,3,{0x90,1,100}},
        {2,3,{0x90,2,99}},
        {4,3,{0x90,3,98}},
        {6,3,{0x90,4,97}},
        {8,3,{0x90,5,96}},

        {14,3,{0x80,1,0}},
        {16,3,{0x80,2,0}},
        {18,3,{0x80,3,0}},
        {20,3,{0x80,4,0}},
        {22,3,{0x80,5,0}},
    };
#endif  
    size_t correct_pckts_idx = 0;
    size_t n_notes = (sizeof(notes)/sizeof(note_info_t));
    size_t n;
    vvvv_tmstmp_t ts = 0;
    vvvv_nt_evnt_t nt_evnts[n_notes];
    vvvv_mnoh_t *mnoh;
    VVVV_MNOH_ALLOC_STACK(mnoh);
    vvvv_mnoh_init(mnoh);
    for (n = 0; n < n_notes; n++) {
        vvvv_nt_evnt_init(&nt_evnts[n], notes[n].ts, notes[n].len, vvvv_nt_evnt_typ_PITCHED);
        nt_evnts[n].dt.pitched.pitch = notes[n].pitch;
        nt_evnts[n].dt.pitched.vel = notes[n].vel;
    }
    while (ts <= (notes[n_notes-1].ts + notes[n_notes-1].len)) {
        for (n = 0; n < n_notes; n++) {
            vvvv_evnt_t *ev = (vvvv_evnt_t*)&nt_evnts[n];
            if (ev->ts == ts) {
                char buf[VVVV_MIDI_PCKT_LST_SZ(2)];
                vvvv_midi_pckt_lst_t *mpl = (vvvv_midi_pckt_lst_t*)buf;
                
                vvvv_evnt_get_midi_pckt_lst(ev,mpl,VVVV_MIDI_PCKT_LST_SZ(2));
#ifdef OSX 
                vvvv_midi_pckt_t *pckt = &mpl->packet[0];
                print_note_packet(pckt);
                passed &= (memcmp(&correct_pckts[correct_pckts_idx++],
                        pckt,
                        ((char*)&pckt->data[3]) - ((char*)pckt)) == 0);
                assert(passed==1);
                pckt = MIDIPacketNext(pckt);
#endif  
                vvvv_mnoh_insert(mnoh, pckt);
            }
        }
        while (mnoh->heap.size) {
            vvvv_mnoh_elem_t *elem;
            elem = (vvvv_mnoh_elem_t*)(mnoh->heap.A[0]);
            if (elem->packet.timeStamp <= ts) {
                HeapErr err;
                err = Heap_pop(&mnoh->heap,(void**)&elem);
                if (!err) {
                    print_note_packet(&elem->packet);
                    passed &= (memcmp(&correct_pckts[correct_pckts_idx++],
                                &elem->packet,
                                ((char*)&elem->packet.data[3]) - ((char*)&elem->packet)) == 0);
                    assert(passed==1);
                }
            } else {
                break;
            }
        }
        ts++;
    }
    return passed;
}

#define MAX_TIMESTAMP 100 

int vvvv_nt_evnt_with_overlaps_test(void)
{
    int passed = 1;
    vvvv_mnoh_t *mnoh;
    VVVV_MNOH_ALLOC_STACK(mnoh);
    vvvv_mnoh_init(mnoh);
    static note_info_t notes[] = {
        {0,14,1,100./127.},
        {2,14,2,99./127.},
        {4,14,3,98./127.},
        {6,14,4,97./127.},
        {8,14,5,96./127.},
        {10,9,1,95./127.},
        {12,2,2,94./127.},
        {14,8,4,93./127.}
    };
#ifdef OSX
    static vvvv_midi_pckt_t correct_pckts[] = {
        {0,3,{0x90,1,100}},
        {2,3,{0x90,2,99}},
        {4,3,{0x90,3,98}},
        {6,3,{0x90,4,97}},
        {8,3,{0x90,5,96}},
        {10,3,{0x90,1,95}},
        {12,3,{0x90,2,94}},
        {14,3,{0x90,4,93}},

        {16,3,{0x80,2,0}},
        {18,3,{0x80,3,0}},
        {19,3,{0x80,1,0}},
        {22,3,{0x80,4,0}},
        {22,3,{0x80,5,0}},
    };
#endif  
    size_t correct_pckts_idx = 0;
    size_t n_notes = (sizeof(notes)/sizeof(note_info_t));
    size_t n;
    vvvv_tmstmp_t ts = 0;
    vvvv_nt_evnt_t nt_evnts[n_notes];
    for (n = 0; n < n_notes; n++) {
        vvvv_nt_evnt_init(&nt_evnts[n], notes[n].ts, notes[n].len, vvvv_nt_evnt_typ_PITCHED);
        nt_evnts[n].dt.pitched.pitch = notes[n].pitch;
        nt_evnts[n].dt.pitched.vel = notes[n].vel;
    }
    /* Note that with new notes checked for first and then the note offs pulled
     * off the heap, notes with the same pitch cannot stop and start at the same
     * time stamp. The heap should be checked first. With the heap checked
     * first, that means no notes can have length 0, which is fine. This just a
     * simple test, so it doesn't matter.
     */
    while (ts <= MAX_TIMESTAMP) {
        for (n = 0; n < n_notes; n++) {
            vvvv_evnt_t *ev = (vvvv_evnt_t*)&nt_evnts[n];
            if (ev->ts == ts) {
                char buf[VVVV_MIDI_PCKT_LST_SZ(2)];
                vvvv_midi_pckt_lst_t *mpl = (vvvv_midi_pckt_lst_t*)buf;
                vvvv_evnt_get_midi_pckt_lst(ev,mpl,VVVV_MIDI_PCKT_LST_SZ(2));
#ifdef OSX 
                vvvv_midi_pckt_t *pckt = &mpl->packet[0];
                print_note_packet(pckt);
                passed &= (memcmp(&correct_pckts[correct_pckts_idx++],
                        pckt,
                        ((char*)&pckt->data[3]) - ((char*)pckt)) == 0);
                assert(passed==1);
                pckt = MIDIPacketNext(pckt);
                vvvv_mnoh_insert(mnoh, pckt);
#endif  
            }
        }
        while (mnoh->heap.size) {
            vvvv_mnoh_elem_t *elem;
            elem = (vvvv_mnoh_elem_t*)(mnoh->heap.A[0]);
            if (elem->packet.timeStamp <= ts) {
                HeapErr err;
                err = Heap_pop(&mnoh->heap,(void**)&elem);
                if (!err) {
                    print_note_packet(&elem->packet);
                    passed &= (memcmp(&correct_pckts[correct_pckts_idx++],
                                &elem->packet,
                                ((char*)&elem->packet.data[3]) - ((char*)&elem->packet)) == 0);
                    assert(passed==1);
                } else {
                    printf("Error: %d\n", (int)err);
                }
            } else {
                break;
            }
        }
        ts++;
    }
    return passed;
}
#undef MAX_TIMESTAMP

// Same as above but tests "new" function
#define MAX_TIMESTAMP 100

static int nwot_n_frees = 0;

static void nwot_nt_evnt_free(vvvv_evnt_t* ev)
{
#if HAVE_MALLOC
    free(ev);
    nwot_n_frees++;
#else
#error "No allocation method defined."
#endif
}

int vvvv_nt_evnt_new_with_overlaps_test(void)
{
    int passed = 1;
    vvvv_mnoh_t *mnoh;
    VVVV_MNOH_ALLOC_STACK(mnoh);
    vvvv_mnoh_init(mnoh);
    static note_info_t notes[] = {
        {0,14,1,100./127.},
        {2,14,2,99./127.},
        {4,14,3,98./127.},
        {6,14,4,97./127.},
        {8,14,5,96./127.},
        {10,9,1,95./127.},
        {12,2,2,94./127.},
        {14,8,4,93./127.}
    };
#ifdef OSX
    static vvvv_midi_pckt_t correct_pckts[] = {
        {0,3,{0x90,1,100}},
        {2,3,{0x90,2,99}},
        {4,3,{0x90,3,98}},
        {6,3,{0x90,4,97}},
        {8,3,{0x90,5,96}},
        {10,3,{0x90,1,95}},
        {12,3,{0x90,2,94}},
        {14,3,{0x90,4,93}},

        {16,3,{0x80,2,0}},
        {18,3,{0x80,3,0}},
        {19,3,{0x80,1,0}},
        {22,3,{0x80,4,0}},
        {22,3,{0x80,5,0}},
    };
#endif  
    size_t correct_pckts_idx = 0;
    size_t n_notes = (sizeof(notes)/sizeof(note_info_t));
    size_t n;
    vvvv_tmstmp_t ts = 0;
    vvvv_nt_evnt_t *nt_evnts[n_notes];
    for (n = 0; n < n_notes; n++) {
        nt_evnts[n] = vvvv_nt_evnt_new(notes[n].ts, notes[n].len, vvvv_nt_evnt_typ_PITCHED);
        vvvv_evnt_set_free(nt_evnts[n],nwot_nt_evnt_free);
        nt_evnts[n]->dt.pitched.pitch = notes[n].pitch;
        nt_evnts[n]->dt.pitched.vel = notes[n].vel;
    }
    /* Note that with new notes checked for first and then the note offs pulled
     * off the heap, notes with the same pitch cannot stop and start at the same
     * time stamp. The heap should be checked first. With the heap checked
     * first, that means no notes can have length 0, which is fine. This just a
     * simple test, so it doesn't matter.
     */
    while (ts <= MAX_TIMESTAMP) {
        for (n = 0; n < n_notes; n++) {
            vvvv_evnt_t *ev = (vvvv_evnt_t*)nt_evnts[n];
            if (ev->ts == ts) {
                char buf[VVVV_MIDI_PCKT_LST_SZ(2)];
                vvvv_midi_pckt_lst_t *mpl = (vvvv_midi_pckt_lst_t*)buf;
                vvvv_evnt_get_midi_pckt_lst(ev,mpl,VVVV_MIDI_PCKT_LST_SZ(2));
#ifdef OSX 
                vvvv_midi_pckt_t *pckt = &mpl->packet[0];
                print_note_packet(pckt);
                passed &= (memcmp(&correct_pckts[correct_pckts_idx++],
                        pckt,
                        ((char*)&pckt->data[3]) - ((char*)pckt)) == 0);
                assert(passed==1);
                pckt = MIDIPacketNext(pckt);
                vvvv_mnoh_insert(mnoh, pckt);
#endif  
            }
        }
        while (mnoh->heap.size) {
            vvvv_mnoh_elem_t *elem;
            elem = (vvvv_mnoh_elem_t*)(mnoh->heap.A[0]);
            if (elem->packet.timeStamp <= ts) {
                HeapErr err;
                err = Heap_pop(&mnoh->heap,(void**)&elem);
                if (!err) {
                    print_note_packet(&elem->packet);
                    passed &= (memcmp(&correct_pckts[correct_pckts_idx++],
                                &elem->packet,
                                ((char*)&elem->packet.data[3]) - ((char*)&elem->packet)) == 0);
                    assert(passed==1);
                } else {
                    printf("Error: %d\n", (int)err);
                }
            } else {
                break;
            }
        }
        ts++;
    }
    /* Free nt_evnts */
    for (n = 0; n < n_notes; n++) {
        vvvv_evnt_free(nt_evnts[n]);
    }
    passed &= (nwot_n_frees == n_notes);
    assert(passed == 1);
    if (passed == 1) {
        printf("Number of frees correct.\n");
    }
    return passed;
}
#undef MAX_TIMESTAMP

int vvvv_nt_evnt_chk_mtch_test(void)
{
    int passed = 1;
    vvvv_nt_evnt_t *ne1;
    ne1 = vvvv_nt_evnt_new_pitched(123u,12u,12.1,10.5);

    if (!ne1) {
        fprintf(stderr,"Error allocating nt_evnt.\n");
        return 0;
    }

    /* Check to see it can find duration */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_DUR,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_UINT,{ .u = 12u}}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_NONE);
        assert(passed == 1);
    }

    /* Check to see it can't find bad duration */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_DUR,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_UINT,{ .u = 13u}}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_NMTCH);
        assert(passed == 1);
    }

    /* Check to see it fails on bad type */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_DUR,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_FLOAT,{ .f = 12.f}}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_EINVAL);
        assert(passed == 1);
    }

    /* Check to see it can find pitch */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_PITCH,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_FLOAT,{ .f = 12.1}}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_NONE);
        assert(passed == 1);
    }

    /* Check to see it can't find bad pitch */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_PITCH,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_FLOAT,{ .f = 12.2}}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_NMTCH);
        assert(passed == 1);
    }

    /* Check to see it fails on bad type */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_PITCH,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_INT,{ .i = 12}}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_EINVAL);
        assert(passed == 1);
    }

    /* Check to see it can find timestamp */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_TS,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_UINT,{ .u = 123u}}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_NONE);
        assert(passed == 1);
    }

    /* Check to see it can't find bad timestamp */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_TS,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_UINT,{ .u = 124u}}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_NMTCH);
        assert(passed == 1);
    }

    /* Check to see it fails on bad type */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_TS,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_FLOAT,{ .f = 12.f}}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_EINVAL);
        assert(passed == 1);
    }

    /* Check to see it can find with multiple properties */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_TS,vvvv_evnt_prm_PITCH,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_UINT,{ .u = 123u }},
                                         {vvvv_evnt_prm_vl_typ_FLOAT,{ .f = 12.1f }}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_NONE);
        assert(passed == 1);
    }

    /* Check to see it can't find with one bad property */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_TS,vvvv_evnt_prm_PITCH,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_UINT,{ .u = 123u }},
                                         {vvvv_evnt_prm_vl_typ_FLOAT,{ .f = 12.2f }}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_NMTCH);
        assert(passed == 1);
    }

    /* Check to see it fails on bad type */
    {
        vvvv_evnt_prm_t ne1_prms[] = {vvvv_evnt_prm_TS,vvvv_evnt_prm_PITCH,vvvv_evnt_prm_END};
        vvvv_evnt_prm_vl_t ne1_vals[] = {{vvvv_evnt_prm_vl_typ_UINT,{ .u = 123u }},
                                         {vvvv_evnt_prm_vl_typ_INT,{ .i = 12 }}};
        passed &= (vvvv_evnt_chk_mtch(ne1,ne1_prms,ne1_vals) == vvvv_err_EINVAL);
        assert(passed == 1);
    }

    vvvv_evnt_free(ne1);
    return passed;
}

int main (void)
{
    int passed = 1;
    passed &= vvvv_nt_evnt_no_overlaps_test();
    printf("\n");
    passed &= vvvv_nt_evnt_with_overlaps_test();
    printf("\n");
    passed &= vvvv_nt_evnt_new_with_overlaps_test();
    printf("\n");
    passed &= vvvv_nt_evnt_chk_mtch_test();
    return -1 * (!passed);
}
