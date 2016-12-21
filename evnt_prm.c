#include "evnt_prm.h"

/* Returns -1, 0 or 1 if a < b, a == b or a > b.
 * If values are different types or unknown, returns -2.
 */
int vvvv_evnt_prm_vl_t_cmp(vvvv_evnt_prm_vl_t *a,
                           vvvv_evnt_prm_vl_t *b)
{
    if (a->typ != b->typ) {
        return -2;
    }
    switch (a->typ) {
        case vvvv_evnt_prm_vl_typ_INT:
            return (a->i > b->i) - (a->i < b->i);
        case vvvv_evnt_prm_vl_typ_UINT:
            return (a->u > b->u) - (a->u < b->u);
        case vvvv_evnt_prm_vl_typ_FLOAT:
            return (a->f > b->f) - (a->f < b->f);
        default:
            return -2;
    }
}
