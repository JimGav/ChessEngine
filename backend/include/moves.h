#ifndef MOVES_H
#define MOVES_H

#include "sqr.h"
#include "types.h"

typedef struct {
    sqr_t origin;
    sqr_t target;
    color_t side;
} Move;

Move *create_move(sqr_t origin, sqr_t target, color_t side);
void destroy_move(Move *move);


#endif