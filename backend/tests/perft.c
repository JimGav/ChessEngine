#include <stdio.h>
#include <stdlib.h>
#include "chess.h"
#include "bitboard.h"

typedef unsigned long long u64;

u64 perft(int depth)
{
    List *list = list_create(compare_moves, NULL);
    int n_moves, i;
    u64 nodes = 0;

    if (depth == 0) 
    return 1ULL;

    ChessState state;
    gen_start_state(&state);
    gen_attck_bbs();
    n_moves = gen_legal_moves(&state, list);

    ListNode *node = list->head;
    while (node){
        Move *move = node->dt_ptr;
        ChessState dummy_state = state;
        play_move(move, &dummy_state);
        nodes += perft(depth - 1);
        node = node->next;
    }

    return nodes;
}


int main(int argc, char *argv[]){
    for (int i = 0; i < atoi(argv[1]); i++)
        printf("PERFT(%d) = %lld\n", i, perft(i));
}