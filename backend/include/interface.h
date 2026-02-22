#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>
#include "chess.h"
#include "sqr.h"


extern ChessState state;


void engine_init();

bool move_legal(sqr_t src, sqr_t target);

/* Make move on global engine state */
void make_move(sqr_t src, sqr_t target);



#endif