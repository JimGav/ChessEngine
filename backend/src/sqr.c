#include <stdbool.h>
#include "sqr.h"


bool valid_sqr(sqr_t sqr){ return sqr >= a1 && sqr <= h8; }


file_t get_rank(sqr_t sqr){ return sqr / 8; }
file_t get_file(sqr_t sqr){ return sqr % 8; }


/* Compass functions */

sqr_t north(sqr_t sqr, int i){ return get_rank(sqr) > 7-i ?  SQR_OUT : (sqr+8*i); }
sqr_t south(sqr_t sqr, int i){ return get_file(sqr) < i   ?  SQR_OUT : (sqr-8*i); }
sqr_t east(sqr_t sqr, int j){ return get_file(sqr) > 7-j  ?  SQR_OUT : (sqr+j); }
sqr_t west(sqr_t sqr, int j){ return get_rank(sqr) < j    ?  SQR_OUT : (sqr-j); }

sqr_t north_west(sqr_t sqr, int i){
    sqr = north(sqr, i);
    if (sqr == SQR_OUT)
        return SQR_OUT;
    return west(sqr, i);
}

sqr_t north_east(sqr_t sqr, int i){
    sqr = north(sqr, i);
    if (sqr == SQR_OUT)
        return SQR_OUT;
    return east(sqr, i);
}

sqr_t south_west(sqr_t sqr, int i){
    sqr = south(sqr, i);
    if (sqr == SQR_OUT)
        return SQR_OUT;
    return west(sqr, i);
}

sqr_t south_east(sqr_t sqr, int i){
    sqr = south(sqr, i);
    if (sqr == SQR_OUT)
        return SQR_OUT;
    return west(sqr, i);
}