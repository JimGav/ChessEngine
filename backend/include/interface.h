#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>
#include "chess.h"
#include "sqr.h"


void engine_init();
bool find_n_play(sqr_t src, sqr_t target);
Move search_move(int depth);


#endif