#ifndef VVVV_COMMON_H
#define VVVV_COMMON_H 

#include <assert.h> 

#if !defined (DEBUG)
#define assert(x) x /* Do nothing */
#endif  

#define VVVV_DONT_CALL \
{ \
    /* Shouldn't be called */ \
   assert(0); \
}


/* Gives the ceiling of the quotient of two integers */
#define VVVV_DIV_CEIL(n,d) (((n) / (d)) + (((n) % (d)) > 0))

/* Gives the rounded result of the quotient of two integers */
#define VVVV_DIV_ROUND(n,d) (((n) + ((d)/2)) / (d)) 

#define VVVV_ABS(x) ((x) < 0) ? (-1*(x)) : (x)

#endif /* VVVV_COMMON_H */
