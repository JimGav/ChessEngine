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

    char files[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    char ranks[] = {'1', '2', '3', '4', '5', '6', '7', '8'};

    char src[3] = {0};
    char target[3] = {0};
    src[0] = files[get_file(move.origin)];
    src[1] = ranks[get_rank(move.origin)];
    target[0] = files[get_file(move.target)];
    target[1] = ranks[get_rank(move.target)];

    switch (move.piece){
        case WHITE_PAWN:
            printf("(%s)->(%s) piece %s\n", src, target, "WHITE_PAWN");
            break;
        case WHITE_KNIGHT:
            printf("(%s)->(%s) piece %s\n", src, target, "WHITE_KNIGHT");
            break;
        case WHITE_BISHOP:
            printf("(%s)->(%s) piece %s\n", src, target, "WHITE_BISHOP");
            break;
        case WHITE_ROOK:
            printf("(%s)->(%s) piece %s\n", src, target, "WHITE_ROOK");
            break;
        case WHITE_KING:
            printf("(%s)->(%s) piece %s\n", src, target, "WHITE_KING");
            break;
        case WHITE_QUEEN:
            printf("(%s)->(%s) piece %s\n", src, target, "WHITE_QUEEN");
            break;
        case BLACK_PAWN:
            printf("(%s)->(%s) piece %s\n", src, target, "BLACK_PAWN");
            break;
        case BLACK_KNIGHT:
            printf("(%s)->(%s) piece %s\n", src, target, "BLACK_KNIGHT");
            break;
        case BLACK_BISHOP:
            printf("(%s)->(%s) piece %s\n", src, target, "BLACK_BISHOP");
            break;
        case BLACK_ROOK:
            printf("(%s)->(%s) piece %s\n", src, target, "BLACK_ROOK");
            break;
        case BLACK_KING:
            printf("(%s)->(%s) piece %s\n", src, target, "BLACK_KING");
            break;
        case BLACK_QUEEN:
            printf("(%s)->(%s) piece %s\n", src, target, "BLACK_QUEEN");
            break;
        default:
            exit(1);
    }
}