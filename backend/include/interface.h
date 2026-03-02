#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>
#include "chess.h"
#include "sqr.h"


extern ChessState state;


void engine_init();

/* Make move on global engine state */
int make_move(sqr_t src, sqr_t target);

Move search_move(int depth);


#endif