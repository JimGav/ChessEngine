#include <stdlib.h>
#include "moves.h"


Move *create_move(sqr_t origin, sqr_t target, color_t side){    
    Move *move = calloc(1, sizeof(*move));
    move->origin = origin;
    move->target = target;
    move->side = side;
    return move;
}

void destroy_move(Move *move){
    assert(move != NULL);
    
    free(move);
    return;
}