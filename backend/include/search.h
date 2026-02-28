#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h>
#include "chess.h"
#include "list.h"

bool is_goal_state(ChessState *state);
int minimax(ChessState *state, int depth, Move *best_move);
int eval(ChessState *state);

#endif