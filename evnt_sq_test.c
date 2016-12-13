#include <stdlib.h> 
#include <stdio.h> 
#include "nt_evnt.h" 
#include "evnt_sq.h" 

struct nt_dat_t {
    float pitch;
    float vel;
    vvvv_tmstmp_t ts;
    vvvv_tmstmp_t len;
    vvvv_nt_evnt_t *ne;
};

struct wp_lst_t {
    vvvv_nt_evnt_t *dat;
    struct wp_lst_t *next;
};

static void vvvv_evnt_free(vvvv_evnt_t *ev)
{
    printf("Freeing event...\n");
    free((void*)ev);
}

static void evnt_func(vvvv_evnt_sq_t *es,
                      size_t idx,
                      vvvv_evnt_t *ev,
                      void *aux)
{
    vvvv_nt_evnt_t *ne = (vvvv_nt_evnt_t*)ev;
    if (ne->typ == vvvv_nt_evnt_typ_PITCHED) {
        printf("pitch: %2d vel: %2d ts: %2llu len: %2llu\n",
                (int)ne->dt.pitched.pitch,
                (int)ne->dt.pitched.vel,
                ev->ts,
                ev->len);
        struct wp_lst_t **wpl = (struct wp_lst_t **)aux;
        struct wp_lst_t *new_wpl = 
            (struct wp_lst_t *)malloc(sizeof(struct wp_lst_t));
        new_wpl->dat = ne;
        new_wpl->next = *wpl;
        *wpl = new_wpl;

    }
}

#define NUM_LIST_ELEMS 3
#define NUM_SEQ_ELEMS 10 
#define SEQ_TICK_DUR 100 
static int evnt_sq_nt_evnt_test(void)
{
    int passed = 1;
    /* Allocate sequence */
    vvvv_evnt_sq_t *es;
    VVVV_EVNT_SEQ_STACK_ALLOC(es,
                              static,
                              NUM_SEQ_ELEMS,
                              NUM_LIST_ELEMS,
                              SEQ_TICK_DUR);
    struct nt_dat_t ntdat[] = {
        {0, 100, 0, 2, NULL},
        {0, 99,  1, 2, NULL},
        {1, 98,  2, 1, NULL},
        {1, 97,  3, 1, NULL} };
    size_t n;
    size_t n_notes = sizeof(ntdat)/sizeof(struct nt_dat_t);
    for (n = 0; n < n_notes; n++) {
        vvvv_nt_evnt_t *ne;
        ne = (vvvv_nt_evnt_t*)malloc(sizeof(vvvv_nt_evnt_t));
        vvvv_nt_evnt_init(ne,
                ntdat[n].ts * SEQ_TICK_DUR,
                ntdat[n].len * SEQ_TICK_DUR, 
                vvvv_nt_evnt_typ_PITCHED);
        ne->dt.pitched.pitch = ntdat[n].pitch;
        ne->dt.pitched.vel = ntdat[n].vel;
        vvvv_evnt_set_free((vvvv_evnt_t*)ne,vvvv_evnt_free);
        size_t idx = ntdat[n].ts;
        if (idx < es->len) {
            vvvv_evnt_lst_add(es->evnt_lsts[idx],
                    (vvvv_evnt_t*)ne);
            ntdat[n].ne = ne;
        } else {
            fprintf(stderr,"Bad timestamp.\n");
            ntdat[n].ne = NULL;
        }
    }
    // Do first 3 events
    struct wp_lst_t *wpl1 = NULL, *wpl_ptr, *tmp;
    vvvv_evnt_sq_mp_rng_t esmr = {evnt_func, (void*)&wpl1 };
    vvvv_evnt_sq_map_range(es, 0, 3 * SEQ_TICK_DUR, &esmr);
    wpl_ptr = wpl1;
    printf("First 3 events.\n");
    // Check to see it correctly extracts the first 3 events
    for (n = 3; n > 0; n--) {
        passed &= ntdat[n-1].ne == wpl_ptr->dat;
        assert(passed);
        wpl_ptr = wpl_ptr->next;
    }
    // Free events
    for (wpl_ptr = wpl1;
         wpl_ptr;
         wpl_ptr = wpl_ptr->next) {
        free(wpl_ptr);
    }
    // ignore 1st event, do next 3
    struct wp_lst_t *wpl2 = NULL;
    esmr.aux = &wpl2;
    vvvv_evnt_sq_map_range(es, 1, 3 * SEQ_TICK_DUR + 1, &esmr);
    wpl_ptr = wpl2;
    printf("Next 3 events.\n");
    // Check to see it correctly extracts the next 3 events
    for (n = 4; n > 1; n--) {
        passed &= ntdat[n-1].ne == wpl_ptr->dat;
        assert(passed);
        wpl_ptr = wpl_ptr->next;
    }
    // Free events
    for (wpl_ptr = wpl2;
         wpl_ptr;
         wpl_ptr = wpl_ptr->next) {
        free(wpl_ptr);
    }

    // Remove events
    for (n = 0; n < n_notes; n++) {
        while (es->evnt_lsts[n]->lst_head) {
            vvvv_evnt_lst_rm_elem(es->evnt_lsts[n],
                    es->evnt_lsts[n]->lst_head,
                    vvvv_evnt_lst_rm_opt_FREVNT);
        }
    }

    return passed;
}

static int evnt_sq_nt_evnt_simultaneous_test(void)
{
    int passed = 1;
    /* Allocate sequence */
    vvvv_evnt_sq_t *es;
    VVVV_EVNT_SEQ_STACK_ALLOC(es,
                              static,
                              NUM_SEQ_ELEMS,
                              NUM_LIST_ELEMS,
                              SEQ_TICK_DUR);
    struct nt_dat_t ntdat[] = {
        {1, 100, 0 * SEQ_TICK_DUR + 1, 2 * SEQ_TICK_DUR, NULL},
        {0, 99,  0 * SEQ_TICK_DUR,     1 * SEQ_TICK_DUR, NULL},
        {2, 101, 0 * SEQ_TICK_DUR,     3 * SEQ_TICK_DUR, NULL},
        // shouldn't play, list will be full
        {3, 102, 0 * SEQ_TICK_DUR,     3 * SEQ_TICK_DUR, NULL}, 
        {0, 99,  1 * SEQ_TICK_DUR - 1, 1 * SEQ_TICK_DUR, NULL},
        {1, 98,  2 * SEQ_TICK_DUR,     1 * SEQ_TICK_DUR, NULL},
        {1, 97,  3 * SEQ_TICK_DUR,     1 * SEQ_TICK_DUR, NULL} };
    size_t n;
    size_t n_notes = sizeof(ntdat)/sizeof(struct nt_dat_t);
    for (n = 0; n < n_notes; n++) {
        vvvv_nt_evnt_t *ne;
        ne = (vvvv_nt_evnt_t*)malloc(sizeof(vvvv_nt_evnt_t));
        vvvv_nt_evnt_init(ne,
                ntdat[n].ts,
                ntdat[n].len, 
                vvvv_nt_evnt_typ_PITCHED);
        ne->dt.pitched.pitch = ntdat[n].pitch;
        ne->dt.pitched.vel = ntdat[n].vel;
        vvvv_evnt_set_free((vvvv_evnt_t*)ne,vvvv_evnt_free);
        vvvv_err_t err;
        if ((err = vvvv_evnt_sq_add(es, (vvvv_evnt_t*)ne))) {
            printf("Not added, err = %d.\n", (int)err);
            ntdat[n].ne = NULL;
        } else {
            ntdat[n].ne = ne;
        }
    }
    // Do first events at indices 2,0,1,4,5
    // First three events in this order because they are sorted by pitch and the
    // highest pitch is at the head of the list
    struct wp_lst_t *wpl1 = NULL, *wpl_ptr, *tmp;
    vvvv_evnt_sq_mp_rng_t esmr = {evnt_func, (void*)&wpl1 };
    vvvv_evnt_sq_map_range(es, 0, 3 * SEQ_TICK_DUR, &esmr);
    wpl_ptr = wpl1;
    printf("Events 2,0,1,4,5.\n");
    size_t evnt_idx[] = {2,0,1,4,5};
    // Check to see it correctly extracts the first 5 events
    for (n = sizeof(evnt_idx)/sizeof(size_t); n > 0; n--) {
        passed &= ntdat[evnt_idx[n-1]].ne == wpl_ptr->dat; 
        assert(passed == 1);
        wpl_ptr = wpl_ptr->next;
    }
    // Free events
    for (wpl_ptr = wpl1;
         wpl_ptr;
         wpl_ptr = wpl_ptr->next) {
        free(wpl_ptr);
    }

    // Remove events
    // Removal order
    size_t evnt_idx2[] = {2, 0, 1, 4, 5, 6}; 
    size_t idx_ = 0;
    for (n = 0; n < es->len; n++) {
        while (es->evnt_lsts[n]->lst_head) {
            vvvv_evnt_t *ev = es->evnt_lsts[n]->lst_head->evnt;
            passed &= ((vvvv_evnt_t*)ntdat[evnt_idx2[idx_++]].ne == ev);
            assert(passed == 1);
            vvvv_evnt_lst_rm_elem(es->evnt_lsts[n],
                    es->evnt_lsts[n]->lst_head,
                    vvvv_evnt_lst_rm_opt_FREVNT);

        }
    }
    
    return passed;
}

int main(void)
{
    int passed = 1;
    passed &= evnt_sq_nt_evnt_test();
    passed &= evnt_sq_nt_evnt_simultaneous_test();
    return -1 * (!passed);
}

