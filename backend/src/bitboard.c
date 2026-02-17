#include <assert.h>
#include "bitboard.h"


BB_t sqr_to_bb(sqr_t sqr){
    return (BB_t)1 << sqr;
}

status_t set_one(BB_t *bb, sqr_t sqr){
    BB_t x = 1;
    *bb = *bb | (x << sqr);

    return STAT_SUCCESS;
}

status_t set_zero(BB_t *bb, sqr_t sqr){
    BB_t x = ~1;
    *bb = *bb & (x << sqr);

    return STAT_SUCCESS;
}


/* Return the square of the lsb with value 1 and zero it out */
sqr_t pop_lsb(BB_t *bb){
    assert(bb != NULL);

    if (*bb == 0)
        return -1;

    sqr_t sqr;
    for (sqr = a1; sqr <= h8; sqr++){
        if (*bb & 1)    break;
        *bb = *bb >> 1;
    }
    *bb = *bb >> 1;
    for (sqr_t sqr2 = sqr; sqr2 != a1; sqr2--){
        *bb = *bb << 1;        
    }
    *bb = *bb << 1;

    return sqr;
}