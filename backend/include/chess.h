#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include "sqr.h"
#include "bitboard.h"
#include "err_handle.h"
#include "list.h"
#include "moves.h"
#include "types.h"


attck_bbs_t attck_bbs;


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

status_t gen_attck_bbs();

/* Move generation */
status_t gen_moves(ChessState *state, List *moves);
status_t gen_pawn_moves(ChessState *state, List *moves);
status_t gen_knight_moves(ChessState *state, List *moves);
status_t gen_bishop_moves(ChessState *state, List *moves);
status_t gen_rook_moves(ChessState *state, List *moves);
status_t gen_queen_moves(ChessState *state, List *moves);
status_t gen_king_moves(ChessState *state, List *moves);
status_t legalize_moves(ChessState *state, List *moves);
status_t play_move(Move *move, ChessState *state);
status_t update_bbs(ChessState *state);
bool in_check(ChessState *state, color_t side);
piece_t get_piece_on_sqr(ChessState *state, sqr_t sqr);


#endif