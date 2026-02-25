#include <stdbool.h>
#include "sqr.h"


bool valid_sqr(sqr_t sqr){ return sqr >= a1 && sqr <= h8; }


file_t get_rank(sqr_t sqr){ return sqr / 8; }
file_t get_file(sqr_t sqr){ return sqr % 8; }


/* Compass functions */

sqr_t north(sqr_t sqr, int i){ return (get_rank(sqr) > 7-i) || sqr == SQR_OUT ?  SQR_OUT : (sqr+8*i); }
sqr_t south(sqr_t sqr, int i){ return (get_rank(sqr) < i)   || sqr == SQR_OUT ?  SQR_OUT : (sqr-8*i); }
sqr_t east(sqr_t sqr, int j){ return (get_file(sqr) > 7-j)  || sqr == SQR_OUT ?  SQR_OUT : (sqr+j); }
sqr_t west(sqr_t sqr, int j){ return (get_file(sqr) < j)    || sqr == SQR_OUT ?  SQR_OUT : (sqr-j); }

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
    return east(sqr, i);
}


void get_neighbours(sqr_t sqr, sqr_t *neighbours){
    neighbours[0] = north(sqr, 1);
    neighbours[1] = north_east(sqr, 1);
    neighbours[2] = east(sqr, 1);
    neighbours[3] = south_east(sqr, 1);
    neighbours[4] = south(sqr, 1);
    neighbours[5] = south_west(sqr, 1);
    neighbours[6] = west(sqr, 1);
    neighbours[7] = north_west(sqr, 1);
}
