#ifndef MOVES_H
#define MOVES_H

#include <assert.h>
#include "sqr.h"
#include "types.h"

typedef struct {
    sqr_t origin;
    sqr_t target;
    color_t side;
    piece_t piece;
} Move;

Move *create_move(sqr_t origin, sqr_t target, color_t side, piece_t piece);
int compare_moves(void *m1, void *m2);
void destroy_move(Move *move);


#endif