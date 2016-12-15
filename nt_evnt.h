#ifndef NT_EVNT_H
#define NT_EVNT_H

#include "evnt.h" 

typedef struct vvvv_nt_evnt_vt_t {
    vvvv_evnt_vt_t super;
} vvvv_nt_evnt_vt_t;

typedef struct vvvv_nt_evnt_t vvvv_nt_evnt_t;

typedef enum {
    vvvv_nt_evnt_typ_UNKNOWN,
    vvvv_nt_evnt_typ_PITCHED,
} vvvv_nt_evnt_typ_t;

typedef struct vvvv_nt_evnt_ptchd_t {
    float pitch;
    float vel;
    /* Not sure if we want this to be same for all instances yet. */
    vvvv_err_t (*get_midi_note_on)(vvvv_nt_evnt_t *, vvvv_midi_pckt_t *);
    vvvv_err_t (*get_midi_note_off)(vvvv_nt_evnt_t *, vvvv_midi_pckt_t *);
} vvvv_nt_evnt_ptchd_t;

typedef union {
    vvvv_nt_evnt_ptchd_t pitched;
} vvvv_nt_evnt_dt_t;

struct vvvv_nt_evnt_t {
    vvvv_evnt_t super;
    vvvv_nt_evnt_typ_t typ;
    vvvv_nt_evnt_dt_t dt;
};

void vvvv_nt_evnt_init(vvvv_nt_evnt_t *ev,
                       vvvv_tmstmp_t ts,
                       vvvv_tmstmp_t len,
                       vvvv_nt_evnt_typ_t type);

#endif /* NT_EVNT_H */
