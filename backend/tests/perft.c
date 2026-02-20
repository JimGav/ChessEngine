#include <stdio.h>
#include <stdlib.h>
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
    for (int i = 0; i < atoi(argv[1]); i++){
        ChessState cstate;
        gen_start_state(&cstate);
        gen_attck_bbs();
        printf("PERFT(%d) =  %lld\n", i, perft(cstate, i));
    }
}