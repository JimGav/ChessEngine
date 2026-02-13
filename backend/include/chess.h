#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include "sqr.h"
#include "bitboard.h"
#include "err_handle.h"
#include "list.h"
#include "moves.h"
#include "types.h"


typedef struct {
    BB_t piece_bbs[12];
    BB_t white_bb, black_bb, all_bb;
    color_t turn;
    bool castling_rights[2];
    sqr_t ep_target;
} ChessState;


/* State search */
status_t gen_start_state(ChessState *start_state);
status_t gen_successors(ChessState *state, List *successors);
bool is_goal_state(ChessState *state);

status_t gen_atck_bbs(attck_bbs_t *bbs);

#endif