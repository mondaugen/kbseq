#ifndef VVVV_COMMON_H
#define VVVV_COMMON_H 

#define VVVV_DONT_CALL(f) \
f \
{ \
#ifdef DEBUG \
    /* Shouldn't be called */ \
   assert(0); \
#endif \
}


/* Gives the ceiling of the quotient of two integers */
#define VVVV_DIV_CEIL(n,d) (((n) / (d)) + (((n) % (d)) > 0))

#endif /* VVVV_COMMON_H */
