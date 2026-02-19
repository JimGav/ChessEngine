#include <stdlib.h>
#include "moves.h"


Move *create_move(sqr_t origin, sqr_t target, color_t side, piece_t piece){    
    Move *move = calloc(1, sizeof(*move));
    assert(move != NULL);
    move->origin = origin;
    move->target = target;
    move->side = side;
    move->piece = piece;
    return move;
}

int compare_moves(void *m1, void *m2){
    assert(m1 != NULL);
    assert(m2 != NULL);

    return m1 == m2;
}

void destroy_move(Move *move){
    assert(move != NULL);
    
    free(move);
    return;
}

void print_move(Move move){
    switch (move.piece){
        case WHITE_PAWN:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "WHITE_PAWN");
            break;
        case WHITE_KNIGHT:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "WHITE_KNIGHT");
            break;
        case WHITE_BISHOP:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "WHITE_BISHOP");
            break;
        case WHITE_ROOK:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "WHITE_ROOK");
            break;
        case WHITE_KING:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "WHITE_KING");
            break;
        case WHITE_QUEEN:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "WHITE_QUEEN");
            break;
        case BLACK_PAWN:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "BLACK_PAWN");
            break;
        case BLACK_KNIGHT:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "BLACK_KNIGHT");
            break;
        case BLACK_BISHOP:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "BLACK_BISHOP");
            break;
        case BLACK_ROOK:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "BLACK_ROOK");
            break;
        case BLACK_KING:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "BLACK_KING");
            break;
        case BLACK_QUEEN:
            printf("(%d)->(%d) piece %s\n", move.origin, move.target, "BLACK_QUEEN");
            break;
        default:
            exit(1);
    }
}