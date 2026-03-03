#include <stdlib.h>
#include "chess.h"
#include "search.h"
#include "types.h"

extern ChessState state;

int minimax(int depth, Move *best_move){
    if (depth == 0  ||  is_goal_state())
        return eval();

    int best = state.turn == WHITE ? INT32_MIN : INT32_MAX;
    List *moves = list_create(compare_moves, destroy_move);
    gen_moves(moves);
    ListNode *node = moves->head;
    while (node){
        Move *move = node->dt_ptr;
        
        ChessState dstate = state;
        if (!make_move(move)){
            node = node->next;
            continue;
        }

        int m = minimax(depth-1, NULL);
        state = dstate;
        
        if (state.turn == WHITE && m > best){
            if (best_move)
                *best_move = *move;
            best = m;
        }
        else if (state.turn == BLACK && m < best){
            if (best_move)
                *best_move = *move;
            best = m;
        }
        
        node = node->next;
    }

    list_destroy(moves);
    return best;
}


int eval(){
    int sum = 0;
    sum += count(state.piece_bbs[WHITE_PAWN]) * PAWN_VAL;
    sum += count(state.piece_bbs[WHITE_KNIGHT]) * KNIGHT_VAL;
    sum += count(state.piece_bbs[WHITE_BISHOP]) * BISHOP_VAL;
    sum += count(state.piece_bbs[WHITE_ROOK]) * ROOK_VAL;
    sum += count(state.piece_bbs[WHITE_QUEEN]) * QUEEN_VAL;
    sum -= count(state.piece_bbs[BLACK_PAWN]) * PAWN_VAL;
    sum -= count(state.piece_bbs[BLACK_KNIGHT]) * KNIGHT_VAL;
    sum -= count(state.piece_bbs[BLACK_BISHOP]) * BISHOP_VAL;
    sum -= count(state.piece_bbs[BLACK_ROOK]) * ROOK_VAL;
    sum -= count(state.piece_bbs[BLACK_QUEEN]) * QUEEN_VAL;
    
    if (in_checkmate(state)){
        sum += (state.turn == WHITE) ? (-KING_VAL) : KING_VAL;
    }

    return sum;
}


bool is_goal_state(){
    return in_checkmate();
}