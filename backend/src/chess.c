#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "chess.h"
#include "sqr.h"
#include "bitboard.h"
#include "err.h"

status_t gen_start_state(ChessState *start_state){

    if (start_state == NULL)
        return STAT_NULLPTR;

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

    for (int i = 0; i < 8; i++){
        set_one(&piece_bbs[WHITE_PAWN], a1+i);
        set_one(&piece_bbs[BLACK_PAWN], a8+i);
    }

    start_state->white_bb = piece_bbs[WHITE_PAWN]   | piece_bbs[WHITE_KNIGHT] 
                          | piece_bbs[WHITE_BISHOP] | piece_bbs[WHITE_ROOK] 
                          | piece_bbs[WHITE_KING]   | piece_bbs[WHITE_QUEEN];

    start_state->black_bb = piece_bbs[BLACK_PAWN]   | piece_bbs[BLACK_KNIGHT] 
                          | piece_bbs[BLACK_BISHOP] | piece_bbs[BLACK_ROOK] 
                          | piece_bbs[BLACK_KING]   | piece_bbs[BLACK_QUEEN];

    start_state->all_bb = start_state->white_bb | start_state->black_bb;

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
    
}


/* 
    Generates an attack bitboard for each piece type and stores it in bbs 
    bbs = bb array of array of bbs for each piece type
*/
extern attck_bbs_t attck_bbs;
status_t gen_attck_bbs(){ //todo: precompute ranks files diagonals

    for (sqr_t sqr = a1; sqr <= h8; sqr++){
        
        /* Knight attack bb */
        attck_bbs.knight_attck[sqr] = 0;
        for (int i = -2; i <= 2; i++){
            for (int j = -2; j <= 2; j++){
                if (abs(i) + abs(j) == 3){
                    sqr_t target = N(sqr, i);
                    if (target != -1) target = W(target, j); 
                    if (target != -1){
                        set_one(&attck_bbs.knight_attck[sqr], sqr);
                    }
                }
            }
        }
        

        /* Bishop attack bb */
        attck_bbs.bishop_attck[sqr] = 0;
        for (int i = 1; i < 7; i++){
            if (NW(sqr, i) != -1)
                set_one(&attck_bbs.bishop_attck[sqr],sqr);
            if (NE(sqr, i) != -1)
                set_one(&attck_bbs.bishop_attck[sqr],sqr);
            if (SW(sqr, i) != -1)
                set_one(&attck_bbs.bishop_attck[sqr],sqr);
            if (SE(sqr, i) != -1)
                set_one(&attck_bbs.bishop_attck[sqr],sqr);
        }
        
        /* Rook attack bb */
        attck_bbs.rook_attck[sqr] = 0;
        for (int i = 1; i < 7; i++){
            if (N(sqr, i) != -1)
                set_one(&attck_bbs.rook_attck[sqr],sqr);
            if (W(sqr, i) != -1)
                set_one(&attck_bbs.rook_attck[sqr],sqr);
            if (E(sqr, i) != -1)
                set_one(&attck_bbs.rook_attck[sqr],sqr);
            if (S(sqr, i) != -1)
                set_one(&attck_bbs.rook_attck[sqr],sqr);
        }
        
        /* King attack bb */
        attck_bbs.king_attck[sqr] = 0;
        if (N(sqr, 1) != -1)
            set_one(&attck_bbs.king_attck[sqr],sqr);
        if (W(sqr, 1) != -1)
            set_one(&attck_bbs.king_attck[sqr],sqr);
        if (E(sqr, 1) != -1)
            set_one(&attck_bbs.king_attck[sqr],sqr);
        if (S(sqr, 1) != -1)
            set_one(&attck_bbs.king_attck[sqr],sqr);
        
        /* Queen attack bb */
        attck_bbs.queen_attck[sqr] = 0;
        for (int i = 1; i < 7; i++){
            
            if (N(sqr, i) != -1)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (W(sqr, i) != -1)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (E(sqr, i) != -1)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (S(sqr, i) != -1)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            
            if (NW(sqr, i) != -1)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (NE(sqr, i) != -1)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (SW(sqr, i) != -1)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
            if (SE(sqr, i) != -1)
                set_one(&attck_bbs.queen_attck[sqr],sqr);
        }
    }
}


/* MOVE GENERATION FUNCTIONS */

/* 
    Generates a list of pseudo legal moves. Pseudo legal moves
    are legal moves as well as moves that leave the king in check.
*/
status_t gen_moves(ChessState *state, List *moves){

    assert(state != NULL);
    assert(moves != NULL);
    
    gen_pawn_moves(state, moves);
    gen_knight_moves(state, moves);
    gen_bishop_moves(state, moves);
    gen_rook_moves(state, moves);
    gen_queen_moves(state, moves);
    gen_king_moves(state, moves);
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
            target = N(src, 1);
            if (target == -1 || (sqr_to_bb(target) & state->all_bb))
                continue;
            Move *move = create_move(src, target, WHITE, WHITE_PAWN);
            list_insert(moves, move);

            target = N(src, 2);
            if (BOARD_FILE(src) != 1 || sqr_to_bb(target) & state->all_bb)
                continue;
            move = create_move(src, target, WHITE, WHITE_PAWN);
            list_insert(moves, move);

            /* Captures */
            target = NW(src, 1);
            if (target != -1 && (sqr_to_bb(target) & state->black_bb)){
                move = create_move(src, target, WHITE, WHITE_PAWN);
                list_insert(moves, move);
            }
            target = NE(src, 1);
            if (target != -1 && (sqr_to_bb(target) & state->black_bb)){
                move = create_move(src, target, WHITE, WHITE_PAWN);
                list_insert(moves, move);
            }

            // todo: en passant
        }
        else if (state->turn == BLACK){
            target = S(src, 1);
            if (target == -1 || (sqr_to_bb(target) & state->all_bb))
                continue;
            Move *move = create_move(src, target, BLACK, BLACK_PAWN);
            list_insert(moves, move);

            target = S(src, 2);
            if (BOARD_FILE(src) != 6 || sqr_to_bb(target) & state->all_bb)
                continue;
            move = create_move(src, target, BLACK, BLACK_PAWN);
            list_insert(moves, move);

            /* Captures */
            target = SW(src, 1);
            if (target != -1 && (sqr_to_bb(target) & state->white_bb)){
                move = create_move(src, target, BLACK, BLACK_PAWN);
                list_insert(moves, move);
            }
            target = SE(src, 1);
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
        while ((target = pop_lsb(&rest_knights)) != -1){
            
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
            target = NE(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }


        for (int i = 1; i < 7; i++){
            target = NW(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 7; i++){
            target = SE(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 7; i++){
            target = SW(src, i);
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
            target = N(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }


        for (int i = 1; i < 7; i++){
            target = W(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 7; i++){
            target = S(src, i);
            if (target == -1 || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 7; i++){
            target = E(src, i);
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
    
    /* Rook moves */ // todo: OPtimize with attck bb  
    for (int i = 1; i < 7; i++){
        target = N(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }


    for (int i = 1; i < 7; i++){
        target = W(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 7; i++){
        target = S(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 7; i++){
        target = E(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    /* Bishop moves */ // todo: OPtimize with attck bb  
    for (int i = 1; i < 7; i++){
        target = NE(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }


    for (int i = 1; i < 7; i++){
        target = NW(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 7; i++){
        target = SE(src, i);
        if (target == -1 || (sqr_to_bb(target) & same_color_bb))
            break;
        
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 7; i++){
        target = SW(src, i);
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
    sqr_t src = sqr_to_bb(king_bb);
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
