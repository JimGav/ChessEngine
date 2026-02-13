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
    BLACK_KING
} piece_t;


typedef struct {
    BB_t knight_attck[64], bishop_attck[64], 
        rook_attck[64], king_attck[64], queen_attck[64];
} attck_bbs_t;



#endif