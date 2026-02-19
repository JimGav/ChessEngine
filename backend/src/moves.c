#include <stdlib.h>
#include "moves.h"


Move *create_move(sqr_t origin, sqr_t target, color_t side, piece_t piece){    
    Move *move = calloc(1, sizeof(*move));
    move->origin = origin;
    move->target = target;
    move->side = side;
    move->piece = piece;
    return move;
}

int compare_moves(void *m1, void *m2){
    assert(m1 != NULL);
    assert(m2 != NULL);

    return m1 == m2;
}

void destroy_move(Move *move){
    assert(move != NULL);
    
    free(move);
    return;
}