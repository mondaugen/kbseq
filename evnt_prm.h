#ifndef EVNT_PRM_H
#define EVNT_PRM_H 

typedef enum {
    vvvv_evnt_prm_END = 0,
    vvvv_evnt_prm_TS,
    vvvv_evnt_prm_DUR,
    vvvv_evnt_prm_PITCH,
} vvvv_evnt_prm_t;

typedef enum {
    vvvv_evnt_prm_vl_typ_UNKNOWN = 0,
    vvvv_evnt_prm_vl_typ_INT,
    vvvv_evnt_prm_vl_typ_UINT,
    vvvv_evnt_prm_vl_typ_FLOAT,
} vvvv_evnt_prm_vl_typ_t;

typedef struct vvvv_evnt_prm_vl_t {
    vvvv_evnt_prm_vl_typ_t typ;
    union {
        float f;
        int i;
        unsigned int u;
    };
} vvvv_evnt_prm_vl_t;

int vvvv_evnt_prm_vl_t_cmp(vvvv_evnt_prm_vl_t *a,
                           vvvv_evnt_prm_vl_t *b);

#endif /* EVNT_PRM_H */
