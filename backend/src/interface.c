#include "chess.h"
#include "interface.h"
#include "search.h"

void engine_init(){
    printf("Initializing engine\n");
    gen_start_state(&state);
    printf("Start state generated successfully\n");
    gen_attck_bbs(&attck_bbs);
    printf("Attack bitboards generated successfully\n");
}


/* 
    Make move on global engine state. Equivalent to
    make_move_on(move, state) where move.origin = src,
    move.target = target and state is the global engine 
    state
*/
int make_move(sqr_t src, sqr_t target){

    /* Find move */
    List *move_list = list_create(compare_moves, destroy_move);
    gen_legal_moves(&state, move_list); //todo: Fix this.really bad
    ListNode *node = move_list->head;
    Move *move;
    while (node){
        move = node->dt_ptr;
        assert(move != NULL);
        if (move->origin == src && move->target == target)
            break;
        node = node->next;
    }

    if (node == NULL) // Move not found
        return -1;
    
    make_move_on(move, &state);
    list_destroy(move_list);

    return 0;
}


Move search_move(int depth){
    Move best_move = {.origin=0,.target=0};
    minimax(&state, depth, &best_move);
    return best_move;
}


int game_result(){
    if (in_checkmate(&state))
        return state.turn == WHITE ? 1 : 0;
    //todo: draws
    return -1;
}