#include "chess.h"
#include "interface.h"
#include "search.h"


extern ChessState state;


void engine_init(){
    printf("Initializing engine\n");
    gen_start_state(&state);
    printf("Start state generated successfully\n");
    gen_attck_bbs(&attck_bbs);
    printf("Attack bitboards generated successfully\n");
}


bool find_n_play(sqr_t src, sqr_t target){
    List *moves = list_create(compare_moves, destroy_move);
    gen_moves(moves);
    Move *move = NULL;
    ListNode *node = moves->head;
    while (node){
        move = node->dt_ptr;
        if (move->origin == src && move->target == target)
            break;
        node = node->next;
    }

    if (move->origin != src || move->target != target){
        printf("MOVE NOT FOUND\n");
        return false;
    }

    bool res = make_move(move);
    if (res == false) printf("MOVE ILLEGAL\n");
    list_destroy(moves);
    return res;
}


Move search_move(int depth){
    Move best_move = {.origin=0,.target=0};
    minimax(depth, &best_move);
    return best_move;
}


int game_result(){
    if (in_checkmate(&state))
        return state.turn == WHITE ? 1 : 0;
    //todo: draws
    return -1;
}