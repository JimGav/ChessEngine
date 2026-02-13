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


/* Generates a list of pseudo legal moves */
status_t gen_moves(ChessState *state, List *moves, attck_bbs_t *attck_bbs){
    
    assert(state != NULL);
    assert(moves != NULL);
    assert(attck_bbs != NULL);
    
    if (state->turn == WHITE){
        
        BB_t bb, attck_bb;
        sqr_t src,target;
        
        /* Knight moves */
        bb = state->piece_bbs[WHITE_KNIGHT];
        attck_bb = attck_bbs->knight_attck[src];
        while ((src = pop_lsb(&bb)) != -1){
            while ((target = pop_lsb(&attck_bb)) != -1){
                Move *move = create_move(src, target, WHITE);
                list_insert(moves, move);
            }
        }
        
        /* Bishop moves */ // todo: OPtimize with attck bb
        bb = state->piece_bbs[WHITE_BISHOP];
        attck_bb = attck_bbs->bishop_attck[src];
        while ((src = pop_lsb(&bb)) != -1){
            for (int i = 1; i < 7; i++){
                target = NE(src, i);
                if (target == -1)
                    continue;
                
                Move *move = create_move(src, target, WHITE);
                list_insert(moves, move);

                if (sqr_to_bb(target) & state->black_bb)
                    break;
            }


            for (int i = 1; i < 7; i++){
                target = NW(src, i);
                if (target == -1)
                    continue;
                
                Move *move = create_move(src, target, WHITE);
                list_insert(moves, move);

                if (sqr_to_bb(target) & state->black_bb)
                    break;
            }

            for (int i = 1; i < 7; i++){
                target = SE(src, i);
                if (target == -1)
                    continue;
                
                Move *move = create_move(src, target, WHITE);
                list_insert(moves, move);

                if (sqr_to_bb(target) & state->black_bb)
                    break;
            }

            for (int i = 1; i < 7; i++){
                target = SW(src, i);
                if (target == -1)
                    continue;
                
                Move *move = create_move(src, target, WHITE);
                list_insert(moves, move);

                if (sqr_to_bb(target) & state->black_bb)
                    break;
            }
        }        
        
    }
    else if (state->turn == BLACK){
    
    }
}




/* 
    Generates an attack bitboard for each piece type and stores it in bbs 
    bbs = bb array of array of bbs for each piece type
*/
status_t gen_atck_bbs(attck_bbs_t *bbs){ //todo: precompute ranks files diagonals
    assert(bbs != NULL);

    for (sqr_t sqr = a1; sqr <= h8; sqr++){
        
        /* Knight attack bb */
        bbs->knight_attck[sqr] = 0;
        for (int i = -2; i <= 2; i++){
            for (int j = -2; j <= 2; j++){
                if (abs(i) + abs(j) == 3){
                    sqr_t target = N(sqr, i);
                    if (target != -1) target = W(target, j); 
                    if (target != -1){
                        set_one(&bbs->knight_attck[sqr], sqr);
                    }
                }
            }
        }
        

        /* Bishop attack bb */
        bbs->bishop_attck[sqr] = 0;
        for (int i = 1; i < 7; i++){
            if (NW(sqr, i) != -1)
                set_one(&bbs->bishop_attck[sqr],sqr);
            if (NE(sqr, i) != -1)
                set_one(&bbs->bishop_attck[sqr],sqr);
            if (SW(sqr, i) != -1)
                set_one(&bbs->bishop_attck[sqr],sqr);
            if (SE(sqr, i) != -1)
                set_one(&bbs->bishop_attck[sqr],sqr);
        }
        
        /* Rook attack bb */
        bbs->rook_attck[sqr] = 0;
        for (int i = 1; i < 7; i++){
            if (N(sqr, i) != -1)
                set_one(&bbs->rook_attck[sqr],sqr);
            if (W(sqr, i) != -1)
                set_one(&bbs->rook_attck[sqr],sqr);
            if (E(sqr, i) != -1)
                set_one(&bbs->rook_attck[sqr],sqr);
            if (S(sqr, i) != -1)
                set_one(&bbs->rook_attck[sqr],sqr);
        }
        
        /* King attack bb */
        bbs->king_attck[sqr] = 0;
        if (N(sqr, 1) != -1)
            set_one(&bbs->king_attck[sqr],sqr);
        if (W(sqr, 1) != -1)
            set_one(&bbs->king_attck[sqr],sqr);
        if (E(sqr, 1) != -1)
            set_one(&bbs->king_attck[sqr],sqr);
        if (S(sqr, 1) != -1)
            set_one(&bbs->king_attck[sqr],sqr);
        
        /* Queen attack bb */
        bbs->queen_attck[sqr] = 0;
        for (int i = 1; i < 7; i++){
            
            if (N(sqr, i) != -1)
                set_one(&bbs->queen_attck[sqr],sqr);
            if (W(sqr, i) != -1)
                set_one(&bbs->queen_attck[sqr],sqr);
            if (E(sqr, i) != -1)
                set_one(&bbs->queen_attck[sqr],sqr);
            if (S(sqr, i) != -1)
                set_one(&bbs->queen_attck[sqr],sqr);
            
            if (NW(sqr, i) != -1)
                set_one(&bbs->queen_attck[sqr],sqr);
            if (NE(sqr, i) != -1)
                set_one(&bbs->queen_attck[sqr],sqr);
            if (SW(sqr, i) != -1)
                set_one(&bbs->queen_attck[sqr],sqr);
            if (SE(sqr, i) != -1)
                set_one(&bbs->queen_attck[sqr],sqr);
        }
    }
}
