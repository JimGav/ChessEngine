#include "bitboard.h"

status_t set_one(BB_t *bb, sqr_t sqr){
    BB_t x = 1;
    *bb = *bb | (x << sqr);
}

status_t set_zero(BB_t *bb, sqr_t sqr){
    BB_t x = ~1;
    *bb = *bb & (x << sqr);
}