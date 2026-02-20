#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "chess.h"
#include "sqr.h"
#include "bitboard.h"
#include "err.h"


/* Global variables */
ChessState state;


/* Backend api */

void engine_init(){
    printf("Initializing engine\n");
    gen_start_state(&state);
    printf("Start state generated successfully\n");
    gen_attck_bbs(&attck_bbs);
    printf("Attack bitboards generated successfully\n");
}

Move *move_search();

/* Returns whether a move is legal */
//todo: optimize
int move_legal(sqr_t src, sqr_t target){
    List *move_list = list_create(&compare_moves, NULL); 
    gen_legal_moves(&state, move_list);
    ListNode *node = move_list->head;
    printf("--- checking %d %d nn in %d moves\n", src, target, move_list->size);
    while (node){
        Move *move = node->dt_ptr;
        print_move(*move);
        node = node->next;
    }
    return 0;
}


status_t play_move(Move *move, ChessState *state){
    assert(move != NULL);
    assert(state != NULL);

    set_zero(&state->piece_bbs[move->piece], move->origin);
    set_one(&state->piece_bbs[move->piece], move->target);

    update_bbs(state);

    return STAT_SUCCESS;
}


status_t gen_start_state(ChessState *start_state){

    assert(start_state != NULL);
    memset(start_state, 0, sizeof(ChessState));
    
    BB_t *piece_bbs = start_state->piece_bbs;

    set_one(&piece_bbs[WHITE_ROOK], a1);
    set_one(&piece_bbs[WHITE_ROOK], h1);
    set_one(&piece_bbs[WHITE_KNIGHT], b1);
    set_one(&piece_bbs[WHITE_KNIGHT], g1);
    set_one(&piece_bbs[WHITE_BISHOP], c1);
    set_one(&piece_bbs[WHITE_BISHOP], f1);
    set_one(&piece_bbs[WHITE_QUEEN], d1);
    set_one(&piece_bbs[WHITE_KING], e1);

    set_one(&piece_bbs[BLACK_ROOK], a8);
    set_one(&piece_bbs[BLACK_ROOK], h8);
    set_one(&piece_bbs[BLACK_KNIGHT], b8);
    set_one(&piece_bbs[BLACK_KNIGHT], g8);
    set_one(&piece_bbs[BLACK_BISHOP], c8);
    set_one(&piece_bbs[BLACK_BISHOP], f8);
    set_one(&piece_bbs[BLACK_QUEEN], d8);
    set_one(&piece_bbs[BLACK_KING], e8);

    for (int j = 0; j < 8; j++){
        set_one(&piece_bbs[WHITE_PAWN], a2+j);
        set_one(&piece_bbs[BLACK_PAWN], a7+j);
    }

    update_bbs(start_state);

    start_state->castling_rights[WHITE] = 0;
    start_state->castling_rights[BLACK] = 0;
    
    start_state->ep_target = 0;
    
    start_state->turn = WHITE;

    return STAT_SUCCESS;
}


/* 
    Given state generates all successor states and stores 
    them in successors list 
*/
status_t gen_successors(ChessState *state, List *successors){
    assert(state != NULL);
    assert(successors != NULL);

    List *move_list = list_create(NULL, NULL); //todo
    gen_legal_moves(state, move_list);
    
    ListNode *node = move_list->head;
    while (node){
        ListNode *next = node->next;

        /* Create successor for current move */
        Move *move = node->dt_ptr;
        ChessState *succ = malloc(sizeof(*succ));
        *succ = *state;
        play_move(move, succ);
        succ->turn = state->turn == WHITE ? BLACK : WHITE;
        list_insert(successors, succ);

        node = next;
    }


    return STAT_SUCCESS;
}


/* 
    Generates an attack bitboard for each piece type and stores it in bbs 
    bbs = bb array of array of bbs for each piece type
*/
attck_bbs_t attck_bbs;
status_t gen_attck_bbs(){ //todo: precompute ranks files diagonals

    for (sqr_t sqr = a1; sqr <= h8; sqr++){
        
        /* Knight attack bb */
        attck_bbs.knight_attck[sqr] = 0;
        sqr_t targets[] = {
            north(west(sqr, 2),1), south(west(sqr, 2), 1), 
            north(east(sqr, 2),1), south(east(sqr, 2), 1), 
            north(west(sqr, 1),2), south(west(sqr, 1), 2), 
            north(east(sqr, 1),2), south(east(sqr, 1), 2), 
        };
        for (int i = 0; i < 8; i++){
            if (targets[i] != SQR_OUT)
                set_one(&attck_bbs.knight_attck[sqr], targets[i]);
        }
        
        

        /* Bishop attack bb */
        attck_bbs.bishop_attck[sqr] = 0;
        for (int i = 1; i < 7; i++){
            if (north_west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.bishop_attck[sqr],sqr);
            if (north_east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.bishop_attck[sqr],sqr);
            if (south_west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.bishop_attck[sqr],sqr);
            if (south_east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.bishop_attck[sqr],sqr);
        }
        
        /* Rook attack bb */
        attck_bbs.rook_attck[sqr] = 0;
        for (int i = 1; i < 7; i++){
            if (north(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.rook_attck[sqr],sqr);
            if (west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.rook_attck[sqr],sqr);
            if (east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.rook_attck[sqr],sqr);
            if (south(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.rook_attck[sqr],sqr);
        }
        
        /* King attack bb */
        attck_bbs.king_attck[sqr] = 0;
        if (north(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],sqr);
        if (west(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],sqr);
        if (east(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],sqr);
        if (south(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],sqr);
        
        /* Queen attack bb */
        attck_bbs.queen_attck[sqr] = 0;
        for (int i = 1; i < 7; i++){
            
            if (north(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (south(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            
            if (north_west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (north_east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (south_west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (south_east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
        }
    }

    return STAT_SUCCESS;
}


/* MOVE GENERATION FUNCTIONS */

/* 
    Generates a list of pseudo legal moves. Pseudo legal moves
    are legal moves as well as moves that leave the king in check.
*/
status_t gen_legal_moves(ChessState *state, List *moves){

    assert(state != NULL);
    assert(moves != NULL);
    
    gen_pawn_moves(state, moves);
    gen_knight_moves(state, moves);
    gen_bishop_moves(state, moves);
    gen_rook_moves(state, moves);
    gen_queen_moves(state, moves);
    gen_king_moves(state, moves);

    legalize_moves(state, moves);
    
    return STAT_SUCCESS;
}


status_t gen_pawn_moves(ChessState *state, List *moves){
    assert(state != NULL);
    assert(moves != NULL);

    //todo: optimize
    BB_t rest_pawns = state->turn == WHITE ?
        state->piece_bbs[WHITE_PAWN]:
        state->piece_bbs[BLACK_PAWN];

    sqr_t src,target;
    while ((src = pop_lsb(&rest_pawns)) != -1){
        if (state->turn == WHITE){
            target = north(src, 1);
            if (target == -1 || (sqr_to_bb(target) & state->all_bb))
                continue;
            Move *move = create_move(src, target, WHITE, WHITE_PAWN);
            list_insert(moves, move);

            target = north(src, 2);
            if (get_rank(src) != 1 || (sqr_to_bb(target) & state->all_bb))
                continue;
            move = create_move(src, target, WHITE, WHITE_PAWN);
            list_insert(moves, move);

            /* Captures */
            target = north_west(src, 1);
            if (target != -1 && (sqr_to_bb(target) & state->black_bb)){
                move = create_move(src, target, WHITE, WHITE_PAWN);
                list_insert(moves, move);
            }
            target = north_east(src, 1);
            if (target != -1 && (sqr_to_bb(target) & state->black_bb)){
                move = create_move(src, target, WHITE, WHITE_PAWN);
                list_insert(moves, move);
            }

            // todo: en passant
        }
        else if (state->turn == BLACK){
            target = south(src, 1);
            if (target == -1 || (sqr_to_bb(target) & state->all_bb))
                continue;
            Move *move = create_move(src, target, BLACK, BLACK_PAWN);
            list_insert(moves, move);

            target = south(src, 2);
            if (get_rank(src) != 6 || (sqr_to_bb(target) & state->all_bb))
                continue;
            move = create_move(src, target, BLACK, BLACK_PAWN);
            list_insert(moves, move);

            /* Captures */
            target = south_west(src, 1);
            if (target != -1 && (sqr_to_bb(target) & state->white_bb)){
                move = create_move(src, target, BLACK, BLACK_PAWN);
                list_insert(moves, move);
            }
            target = south_east(src, 1);
            if (target != -1 && (sqr_to_bb(target) & state->white_bb)){
                move = create_move(src, target, BLACK, BLACK_PAWN);
                list_insert(moves, move);
            }
            // todo: en passant
        }
    }

    return STAT_SUCCESS;
}


status_t gen_knight_moves(ChessState *state, List *moves){
    assert(state != NULL);
    assert(moves != NULL);

    BB_t rest_knights = state->turn == WHITE ? 
        state->piece_bbs[WHITE_KNIGHT]: 
        state->piece_bbs[BLACK_KNIGHT];
    BB_t same_color_bb = state->turn == WHITE ?
        state->white_bb:
        state->black_bb;

    sqr_t src, target;
    while ((src = pop_lsb(&rest_knights)) != -1){
        BB_t rest_attcks = attck_bbs.knight_attck[src];
        while ((target = pop_lsb(&rest_attcks)) != -1){
            
            /* Target occupied by same color */
            if (sqr_to_bb(target) & same_color_bb)
                continue;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT);
            list_insert(moves, move);
        }
    }

    return STAT_SUCCESS;
}

status_t gen_bishop_moves(ChessState *state, List *moves){
    assert(state != NULL);
    assert(moves != NULL);

    BB_t rest_bishops = state->turn == WHITE ? 
        state->piece_bbs[WHITE_BISHOP]: 
        state->piece_bbs[BLACK_BISHOP];
    BB_t same_color_bb = state->turn == WHITE ?
        state->white_bb:
        state->black_bb;
    BB_t opp_color_bb = state->turn == WHITE ?
        state->black_bb:
        state->white_bb;


    /* Bishop moves */ // todo: OPtimize with attck bb
    sqr_t src, target;
    while ((src = pop_lsb(&rest_bishops)) != -1){
        
        for (int i = 1; i < 7; i++){
            target = north_east(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }


        for (int i = 1; i < 7; i++){
            target = north_west(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 7; i++){
            target = south_east(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 7; i++){
            target = south_west(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }
    }    

    return STAT_SUCCESS;
}

status_t gen_rook_moves(ChessState *state, List *moves){
    assert(state != NULL);
    assert(moves != NULL);

    BB_t rest_rooks = state->turn == WHITE ? 
        state->piece_bbs[WHITE_KNIGHT]: 
        state->piece_bbs[BLACK_KNIGHT];
    BB_t same_color_bb = state->turn == WHITE ?
        state->white_bb:
        state->black_bb;
    BB_t opp_color_bb = state->turn == WHITE ?
        state->black_bb:
        state->white_bb;


    /* Rook moves */ // todo: OPtimize with attck bb
    sqr_t src, target;
    while ((src = pop_lsb(&rest_rooks)) != -1){
        
        for (int i = 1; i < 7; i++){
            target = north(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }


        for (int i = 1; i < 7; i++){
            target = west(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 7; i++){
            target = south(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 7; i++){
            target = east(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }
    }    

    return STAT_SUCCESS;
}

status_t gen_queen_moves(ChessState *state, List *moves){
    assert(state != NULL);
    assert(moves != NULL);

    BB_t same_color_bb = state->turn == WHITE ?
        state->white_bb:
        state->black_bb;
    BB_t opp_color_bb = state->turn == WHITE ?
        state->black_bb:
        state->white_bb;


    sqr_t src, target;
    src = pop_lsb(&state->piece_bbs[WHITE_QUEEN]);
    /* Rook moves */ // todo: OPtimize with attck bb  
    for (int i = 1; i < 7; i++){
        target = north(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }


    for (int i = 1; i < 7; i++){
        target = west(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 7; i++){
        target = south(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 7; i++){
        target = east(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    /* Bishop moves */ // todo: OPtimize with attck bb  
    for (int i = 1; i < 7; i++){
        target = north_east(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }


    for (int i = 1; i < 7; i++){
        target = north_west(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 7; i++){
        target = south_east(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 7; i++){
        target = south_west(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    return STAT_SUCCESS;
}

status_t gen_king_moves(ChessState *state, List *moves){
    assert(state != NULL);
    assert(moves != NULL);

    BB_t king_bb = state->turn == WHITE ?
        state->piece_bbs[WHITE_KING] :
        state->piece_bbs[BLACK_KING];
    BB_t same_color_bb = state->turn == WHITE ?
        state->white_bb:
        state->black_bb;
        
    /* King moves */ // todo: optimize
    BB_t rest_kings = king_bb;
    sqr_t src = pop_lsb(&rest_kings);
    sqr_t target;
    BB_t rest_attcks = attck_bbs.king_attck[src];
    while ((target = pop_lsb(&rest_attcks)) != -1){ 
        /* Target occupied by same color piece */
        if (sqr_to_bb(target) & same_color_bb)
            continue;
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_KING : BLACK_KING);
        list_insert(moves, move);
    } 

    return STAT_SUCCESS;
}


/* Removes all remaining legal moves from move list */
status_t legalize_moves(ChessState *state, List *moves){
    assert(state != NULL);
    assert(moves != NULL);

    
    ListNode *node = moves->head;
    while (node != NULL){
        ListNode *next = node->next;
        Move *move = node->dt_ptr;

        /* Simultate move on a dummy state */
        ChessState dummy_state = *state;
        color_t turn = state->turn;
        play_move(move, &dummy_state);
        
        /* If after playing move player is in check remove */
        if (in_check(state, turn))
            list_remove(moves, move);
        
        node = next;
    }

    return STAT_SUCCESS;
}



status_t update_bbs(ChessState *state){
    assert(state != NULL);

    state->white_bb = state->piece_bbs[WHITE_PAWN]   | state->piece_bbs[WHITE_KNIGHT] 
                    | state->piece_bbs[WHITE_BISHOP] | state->piece_bbs[WHITE_ROOK] 
                    | state->piece_bbs[WHITE_KING]   | state->piece_bbs[WHITE_QUEEN];

    state->black_bb = state->piece_bbs[BLACK_PAWN]   | state->piece_bbs[BLACK_KNIGHT] 
                    | state->piece_bbs[BLACK_BISHOP] | state->piece_bbs[BLACK_ROOK] 
                    | state->piece_bbs[BLACK_KING]   | state->piece_bbs[BLACK_QUEEN];

    state->all_bb = state->white_bb | state->black_bb;
    
    return STAT_SUCCESS;
}

bool in_check(ChessState *state, color_t side){
    assert(state != NULL);

    if (side == WHITE){
        BB_t rest_black = state->black_bb;
        sqr_t src;
        while ((src = pop_lsb(&rest_black)) != -1){
            piece_t piece = get_piece_on_sqr(state, src);
            if (piece == BLACK_KNIGHT && (attck_bbs.knight_attck[src] & state->piece_bbs[WHITE_KING]))
                return true;
            if (piece == BLACK_BISHOP && (attck_bbs.bishop_attck[src] & state->piece_bbs[WHITE_KING]))
                return true;
            if (piece == BLACK_ROOK && (attck_bbs.rook_attck[src] & state->piece_bbs[WHITE_KING]))
                return true;
            if (piece == BLACK_QUEEN && (attck_bbs.queen_attck[src] & state->piece_bbs[WHITE_KING]))
                return true;
            if (piece == BLACK_KING && (attck_bbs.king_attck[src] & state->piece_bbs[WHITE_KING]))
                return true;
        }
    }
    else if (side == BLACK){

    }
    
    return STAT_SUCCESS;
}


piece_t get_piece_on_sqr(ChessState *state, sqr_t sqr){
    assert(state != NULL);
    BB_t sqr_bb = sqr_to_bb(sqr);

    for (piece_t i = 0; i < PIECE_T_LAST; i++){
        if (state->piece_bbs[i] & sqr_bb)
            return i;
    }

    return PIECE_T_LAST;
}
