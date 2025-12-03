#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include "sqr.h"
#include "bitboard.h"
#include "err_handle.h"
#include "list.h"


typedef enum {
    WHITE, BLACK
} color_t;


typedef enum {
    WHITE_PAWN,
    WHITE_ROOK,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN,
    BLACK_ROOK,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_QUEEN,
    BLACK_KING
} piece_t;


typedef struct {
    BB_t piece_bbs[12];
    color_t turn;
    bool castling_rights[2];
    sqr_t ep_target;
} ChessState;


/* State search */
status_t gen_start_state(ChessState *start_state);
status_t gen_successors(ChessState *state, List *successors);
bool is_goal_state(ChessState *state);

#endif