#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "chess.h"
#include "bitboard.h"

typedef unsigned long long u64;



u64 perft(ChessState state, int depth)
{
    if (depth == 0)
        return 1ULL;

    List *succ = list_create(NULL, NULL);
    gen_successors(&state, succ);
    
    int n = 0;
    for (int i = 0; i < succ->size; i++){
        ChessState s = *((ChessState*)succ->head->dt_ptr);
        n += perft(s, depth-1);
        succ->head = succ->head->next;
    }

    return n;
}


int main(int argc, char *argv[]){

    u64 perft_res[] = {
        1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956, 2439530234167, 69352859712417
    };

    for (int i = 0; i < atoi(argv[1]); i++){
        ChessState cstate;
        gen_start_state(&cstate);
        gen_attck_bbs();
        
        u64 p = perft(cstate, i);
        printf("PERFT(%d) =  %lld\n", i, p);
        assert(p == perft_res[i]);
    }
}