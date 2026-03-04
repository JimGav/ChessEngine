#ifndef TYPES_H
#define TYPES_H

#include "bitboard.h"


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
    BLACK_KING,
    PIECE_T_LAST
} piece_t;


/* Piece values for eval */
#define PAWN_VAL    100
#define KNIGHT_VAL  320
#define BISHOP_VAL  330
#define ROOK_VAL    500
#define QUEEN_VAL   900
#define KING_VAL    20000


typedef struct {
    BB_t knight_attck[64], bishop_attck[64], 
        rook_attck[64], king_attck[64], queen_attck[64],
        white_pawn_capt[64], black_pawn_capt[64];
} attck_bbs_t;



#endif