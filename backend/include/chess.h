#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include "bitboard.h"
#include "err_handle.h"
#include "list.h"


typedef enum {
    WHITE, BLACK
} color_t;


typedef enum {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8
} sqr_t;


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