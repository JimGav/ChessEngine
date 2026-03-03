#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include "sqr.h"
#include "bitboard.h"
#include "err_handle.h"
#include "list.h"
#include "moves.h"
#include "types.h"


extern attck_bbs_t attck_bbs;


typedef struct {
    BB_t piece_bbs[12];
    BB_t white_bb, black_bb, all_bb;
    color_t turn;
    bool castling_rights[2];
    sqr_t ep_target;
} ChessState;


status_t gen_start_state();
status_t gen_attck_bbs();

/* Move generation */
status_t gen_moves(List *moves);
status_t gen_pawn_moves(List *moves);
status_t gen_knight_moves(List *moves);
status_t gen_bishop_moves(List *moves);
status_t gen_rook_moves(List *moves);
status_t gen_queen_moves(List *moves);
status_t gen_king_moves(List *moves);
status_t legalize_moves(List *moves);
bool make_move(Move *move);
status_t update_bbs();
bool in_check(color_t side);
bool in_checkmate();
piece_t get_piece_on_sqr(sqr_t sqr);
bool targets(color_t turn, sqr_t sqr);

#endif