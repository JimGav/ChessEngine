#include <stdlib.h>
#include "chess.h"
#include "search.h"
#include "types.h"


int minimax(ChessState *state, int depth, Move *best_move){
    if (depth == 0  ||  is_goal_state(state))
        return eval(state);

    int best;
    List *moves = list_create(compare_moves, destroy_move);
    gen_legal_moves(state, moves);
    ListNode *node = moves->head;
    while (node){
        Move *move = node->dt_ptr;
        
        ChessState dstate = *state;
        make_move_on(move, &dstate);
        int m = minimax(&dstate, depth-1, NULL); 
        if (node == moves->head){
            if (best_move)
                *best_move = *move;
            best = m;
        }

        if (state->turn == WHITE && m > best){
            if (best_move)
                *best_move = *move;
            best = m;
        }
        else if (state->turn == BLACK && m < best){
            if (best_move)
                *best_move = *move;
            best = m;
        }
        
        node = node->next;
    }

    list_destroy(moves);
    return best;
}


int eval(ChessState *state){
    int sum = 0;
    sum += count(state->piece_bbs[WHITE_PAWN]) * PAWN_VAL;
    sum += count(state->piece_bbs[WHITE_KNIGHT]) * KNIGHT_VAL;
    sum += count(state->piece_bbs[WHITE_BISHOP]) * BISHOP_VAL;
    sum += count(state->piece_bbs[WHITE_ROOK]) * ROOK_VAL;
    sum += count(state->piece_bbs[WHITE_QUEEN]) * QUEEN_VAL;
    sum -= count(state->piece_bbs[BLACK_PAWN]) * PAWN_VAL;
    sum -= count(state->piece_bbs[BLACK_KNIGHT]) * KNIGHT_VAL;
    sum -= count(state->piece_bbs[BLACK_BISHOP]) * BISHOP_VAL;
    sum -= count(state->piece_bbs[BLACK_ROOK]) * ROOK_VAL;
    sum -= count(state->piece_bbs[BLACK_QUEEN]) * QUEEN_VAL;
    
    if (in_checkmate(state)){
        sum += (state->turn == WHITE) ? (-KING_VAL) : KING_VAL;
    }

    return sum;
}


bool is_goal_state(ChessState *state){
    return in_checkmate(state);
}