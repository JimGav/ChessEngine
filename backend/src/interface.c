#include "chess.h"
#include "interface.h"


void engine_init(){
    printf("Initializing engine\n");
    gen_start_state(&state);
    printf("Start state generated successfully\n");
    gen_attck_bbs(&attck_bbs);
    printf("Attack bitboards generated successfully\n");
}


bool move_legal(sqr_t src, sqr_t target){
    
    /* Find move src->target */
    List *move_list = list_create(&compare_moves, NULL); 
    gen_legal_moves(&state, move_list); //todo: dont gen moves
    ListNode *node = move_list->head;
    while (node){
        Move *move = node->dt_ptr;
        assert(move != NULL);
        if (move->origin == src && move->target == target){
            return true;
        }
        node = node->next;
    }    

    return false;
}


/* 
    Make move on global engine state. Equivalent to
    make_move_on(move, state) where move.origin = src,
    move.target = target and state is the global engine 
    state
*/
void make_move(sqr_t src, sqr_t target){

    /* Find move */
    List *move_list = list_create(compare_moves, NULL);
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

    make_move_on(move, &state);
}