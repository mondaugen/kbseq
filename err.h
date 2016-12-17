#ifndef ERR_H
#define ERR_H 

typedef enum {
    vvvv_err_NONE = 0,
    vvvv_err_EINVAL,    /* Invalid */
    vvvv_err_NOTIMP,    /* Not implemented */
    vvvv_err_EFULL,     /* Full */
    vvvv_err_EBNDS,     /* Out of bounds */
    vvvv_err_NMTCH,     /* Doesn't match */
} vvvv_err_t;


#endif /* ERR_H */
