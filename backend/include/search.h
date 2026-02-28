#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>
#include "chess.h"
#include "list.h"

int minimax(ChessState *state, int depth);

#endif