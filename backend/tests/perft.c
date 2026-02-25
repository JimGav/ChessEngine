#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "chess.h"
#include "bitboard.h"

typedef unsigned long long u64;



u64 perft(ChessState state, int depth, int *captures, int *ep, int *castles)
{
    List *moves = list_create(compare_moves, destroy_move);
    gen_legal_moves(&state, moves);
    
    if (depth == 0){
        list_destroy(moves);
        return 1ULL;
    }

    int n = 0;
    ListNode *node = moves->head;
    for (int i = 0; i < moves->size; i++){
        Move *move = node->dt_ptr;

        if (depth == 1){
            if (move->castling != NO_CASTLING)
                *castles = *castles+1;
            if (get_piece_on_sqr(&state, move->target) != PIECE_T_LAST)
                *captures = *captures+1;
            if (move->is_ep){print_move(*move);
                *ep = *ep+1;}

        }

        ChessState dstate = state;
        make_move_on(move, &dstate);
        n += perft(dstate, depth-1, captures, ep, castles);
        node = node->next;
    }

    list_destroy(moves);
    return n;
}


int main(int argc, char *argv[]){

    u64 perft_res[] = {
        1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956, 2439530234167, 69352859712417
    };
    u64 perft_captures[] = {
        0, 0, 0, 34, 1576, 82719, 2812008, 108329926, 3523740106, 125208536153
    };
    u64 perft_castles[] = {
        0, 0, 0, 0, 0, 0, 0, 883453, 23605205, 1784356000	
    };
    u64 perft_ep[] = {
        0, 0, 0, 0, 0, 258, 5248, 319617, 7187977, 319496827
    };

    for (int i = 0; i <= atoi(argv[1]); i++){
        ChessState cstate;
        gen_start_state(&cstate);
        gen_attck_bbs();
        
        int captures = 0;
        int castles = 0;
        int ep = 0;
        u64 p = perft(cstate, i, &captures, &ep, &castles);
        printf("PERFT(%d) =  %lld Captures:%d, EP:%d, Castles:%d\n", i, p, captures, ep, castles);
        assert(p == perft_res[i]);
        assert(captures == perft_captures[i]);
        assert(ep == perft_ep[i]);
        assert(castles == perft_castles[i]);
    }
}