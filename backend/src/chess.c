#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "chess.h"
#include "sqr.h"
#include "bitboard.h"
#include "err.h"


/* Global variables */
ChessState state;


status_t make_move_on(Move *move, ChessState *state){
    assert(state != NULL);

    /* Update captured piece bb */
    if (!move->is_ep){
        piece_t captured = get_piece_on_sqr(state, move->target);
        set_zero(&state->piece_bbs[captured], move->target);
    }
    else {
        piece_t captured = get_piece_on_sqr(state, state->ep_target);
        set_zero(&state->piece_bbs[captured], state->ep_target);
    }   
    
    /* Update moving piece bb */
    set_zero(&state->piece_bbs[move->piece], move->origin);
    set_one(&state->piece_bbs[move->piece], move->target);

    /* If castling update rook bb as well */
    if (move->piece == WHITE_KING && move->castling != NO_CASTLING){
        sqr_t rook_src = move->castling == KINGSIDE ? h1 : a1;
        sqr_t rook_target = move->castling == KINGSIDE ? f1 : d1;
        set_zero(&state->piece_bbs[WHITE_ROOK], rook_src);
        set_one(&state->piece_bbs[WHITE_ROOK], rook_target);
    }
    if (move->piece == BLACK_KING && move->castling != NO_CASTLING){
        sqr_t rook_src = move->castling == KINGSIDE ? h8 : a8;
        sqr_t rook_target = move->castling == KINGSIDE ? f8 : d8;
        set_zero(&state->piece_bbs[BLACK_ROOK], rook_src);
        set_one(&state->piece_bbs[BLACK_ROOK], rook_target);
    }
    
    /* Update castling rights */
    if (move->piece == WHITE_KING || move->piece == BLACK_KING)
        state->castling_rights[move->side] = false;

    update_bbs(state);

    /* Update state->ep_target */
    if (double_pawn_move(move)){
        state->ep_target = (move->side == WHITE) ? 
            south(move->target, 1):
            north(move->target, 1);
    }
    else
        state->ep_target = SQR_OUT;
    
    /* Update turn */
    state->turn = (state->turn == WHITE) ? BLACK : WHITE;
    
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

    start_state->castling_rights[WHITE] = true;
    start_state->castling_rights[BLACK] = true;
    
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

    List *move_list = list_create(compare_moves, destroy_move);
    gen_legal_moves(state, move_list);
    
    ListNode *node = move_list->head;
    while (node){
        ListNode *next = node->next;

        /* Create successor for current move */
        Move *move = node->dt_ptr;
        ChessState *succ = malloc(sizeof(*succ));
        *succ = *state;
        make_move_on(move, succ);
        succ->turn = state->turn == WHITE ? BLACK : WHITE;
        list_insert(successors, succ);

        node = next;
    }

    list_destroy(move_list);
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
        for (int i = 1; i < 8; i++){
            if (north_west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.bishop_attck[sqr],north_west(sqr,i));
            if (north_east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.bishop_attck[sqr],north_east(sqr, i));
            if (south_west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.bishop_attck[sqr],south_west(sqr, i));
            if (south_east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.bishop_attck[sqr],south_east(sqr, i));
        }
        
        /* Rook attack bb */
        attck_bbs.rook_attck[sqr] = 0;
        for (int i = 1; i < 8; i++){
            if (north(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.rook_attck[sqr],north(sqr, i));
            if (west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.rook_attck[sqr],west(sqr, i));
            if (east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.rook_attck[sqr],east(sqr, i));
            if (south(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.rook_attck[sqr],south(sqr, i));
        }
        
        /* King attack bb */
        attck_bbs.king_attck[sqr] = 0;
        if (north(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],north(sqr,1));
        if (west(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],west(sqr,1));
        if (east(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],east(sqr,1));
        if (south(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],south(sqr,1));
        if (north_west(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],north_west(sqr,1));
        if (north_east(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],north_east(sqr,1));
        if (south_west(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],south_west(sqr,1));
        if (south_east(sqr, 1) != SQR_OUT)
            set_one(&attck_bbs.king_attck[sqr],south_east(sqr,1));
        
        /* Queen attack bb */
        attck_bbs.queen_attck[sqr] = 0;
        for (int i = 1; i < 8; i++){
            
            if (north(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],north(sqr, i));
            if (west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],west(sqr, i));
            if (east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],east(sqr, i));
            if (south(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],south(sqr, i));
            
            if (north_west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],north_west(sqr, i));
            if (north_east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],north_east(sqr, i));
            if (south_west(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],south_west(sqr, i));
            if (south_east(sqr, i) != SQR_OUT)
                set_one(&attck_bbs.queen_attck[sqr],south_east(sqr, i));
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
            
            /* Single pawn push */
            target = north(src, 1);
            if (target == SQR_OUT)
                continue;

            Move *move;
            if (!(sqr_to_bb(target) & state->all_bb)){
                move = create_move(src, target, WHITE, WHITE_PAWN, false, NO_CASTLING);
                list_insert(moves, move);
            }

            /* Double pawn push */
            sqr_t target2 = north(src, 2);
            if ((get_rank(src) == 1) && !(sqr_to_bb(target) & state->all_bb) && !(sqr_to_bb(target2) & state->all_bb)){
                move = create_move(src, target2, WHITE, WHITE_PAWN, false, NO_CASTLING);
                list_insert(moves, move);
            }

            /* Captures */
            target = north_west(src, 1);
            if (target != SQR_OUT && ((sqr_to_bb(target) & state->black_bb)  || (target == state->ep_target))){
                move = create_move(src, target, WHITE, WHITE_PAWN, target == state->ep_target, NO_CASTLING);
                list_insert(moves, move);
            }
            target = north_east(src, 1);
            if (target != SQR_OUT && ((sqr_to_bb(target) & state->black_bb ) || (target == state->ep_target))){
                move = create_move(src, target, WHITE, WHITE_PAWN, target == state->ep_target, NO_CASTLING);
                list_insert(moves, move);
            }        
        }
        else if (state->turn == BLACK){
            
            /* Single pawn push */
            target = south(src, 1);
            if (target == SQR_OUT)
                continue;

            Move *move;
            if (!(sqr_to_bb(target) & state->all_bb)){
                move = create_move(src, target, BLACK, BLACK_PAWN, false, NO_CASTLING);
                list_insert(moves, move);
            }

            /* Double pawn push */
            sqr_t target2 = south(src, 2);
            if ((get_rank(src) == 6) && !(sqr_to_bb(target) & state->all_bb) && !(sqr_to_bb(target2) & state->all_bb)){
                move = create_move(src, target2, BLACK, BLACK_PAWN, false, NO_CASTLING);
                list_insert(moves, move);
            }

            /* Captures */
            target = south_west(src, 1);
            if (target != SQR_OUT && ((sqr_to_bb(target) & state->white_bb)  || (target == state->ep_target))){
                move = create_move(src, target, BLACK, BLACK_PAWN, target == state->ep_target, NO_CASTLING);
                list_insert(moves, move);
            }
            target = south_east(src, 1);
            if (target != SQR_OUT && ((sqr_to_bb(target) & state->white_bb ) || (target == state->ep_target))){
                move = create_move(src, target, BLACK, BLACK_PAWN, target == state->ep_target, NO_CASTLING);
                list_insert(moves, move);
            }
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
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT, false, NO_CASTLING);
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
        
        for (int i = 1; i < 8; i++){
            target = north_east(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP, false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }


        for (int i = 1; i < 8; i++){
            target = north_west(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP, false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 8; i++){
            target = south_east(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP, false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 8; i++){
            target = south_west(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP, false, NO_CASTLING);
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
        state->piece_bbs[WHITE_ROOK]: 
        state->piece_bbs[BLACK_ROOK];
    BB_t same_color_bb = state->turn == WHITE ?
        state->white_bb:
        state->black_bb;
    BB_t opp_color_bb = state->turn == WHITE ?
        state->black_bb:
        state->white_bb;


    /* Rook moves */ // todo: OPtimize with attck bb
    sqr_t src, target;
    while ((src = pop_lsb(&rest_rooks)) != -1){
        
        for (int i = 1; i < 8; i++){
            target = north(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK, false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }


        for (int i = 1; i < 8; i++){
            target = west(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK, false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 8; i++){
            target = south(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK, false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 8; i++){
            target = east(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_ROOK : BLACK_ROOK, false, NO_CASTLING);
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

    BB_t rest_queens = state->turn == WHITE ?
        state->piece_bbs[WHITE_QUEEN]:
        state->piece_bbs[BLACK_QUEEN];

    sqr_t src, target;
    src = pop_lsb(&rest_queens);
    if (src == -1)
        return STAT_SUCCESS;

    /* Rook moves */ // todo: OPtimize with attck bb  
    for (int i = 1; i < 8; i++){
        target = north(src, i);
        if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
            break;
        
        assert(valid_sqr(src));
        assert(valid_sqr(target));
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, false, NO_CASTLING);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }


    for (int i = 1; i < 8; i++){
        target = west(src, i);
        if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
            break;
        
        assert(valid_sqr(src));
        assert(valid_sqr(target));
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, false, NO_CASTLING);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 8; i++){
        target = south(src, i);
        if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
            break;
        
        assert(valid_sqr(src));
        assert(valid_sqr(target));
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, false, NO_CASTLING);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 8; i++){
        target = east(src, i);
        if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
            break;
        
        assert(valid_sqr(src));
        assert(valid_sqr(target));
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, false, NO_CASTLING);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    /* Bishop moves */ // todo: OPtimize with attck bb  
    for (int i = 1; i < 8; i++){
        target = north_east(src, i);
        if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
            break;
        
        assert(valid_sqr(src));
        assert(valid_sqr(target));
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, false, NO_CASTLING);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }


    for (int i = 1; i < 8; i++){
        target = north_west(src, i);
        if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
            break;
        
        assert(valid_sqr(src));
        assert(valid_sqr(target));
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, false, NO_CASTLING);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 8; i++){
        target = south_east(src, i);
        if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
            break;
        
        assert(valid_sqr(src));
        assert(valid_sqr(target));
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, false, NO_CASTLING);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    for (int i = 1; i < 8; i++){
        target = south_west(src, i);
        if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
            break;
        
        assert(valid_sqr(src));
        assert(valid_sqr(target));
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, false, NO_CASTLING);
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
        assert(valid_sqr(src));
        assert(valid_sqr(target));
        Move *move = create_move(src, target, state->turn, state->turn == WHITE ? WHITE_KING : BLACK_KING, false, NO_CASTLING);
        list_insert(moves, move);
    } 
    
    /* Castling */
    if (state->turn == WHITE && state->castling_rights[WHITE] && !in_check(state, WHITE)){

        assert(src == e1);

        piece_t apiece = get_piece_on_sqr(state, a1);
        piece_t bpiece = get_piece_on_sqr(state, b1);
        piece_t cpiece = get_piece_on_sqr(state, c1);
        piece_t dpiece = get_piece_on_sqr(state, d1);
        piece_t fpiece = get_piece_on_sqr(state, f1);
        piece_t gpiece = get_piece_on_sqr(state, g1);
        piece_t hpiece = get_piece_on_sqr(state, h1);

        /* Kingside castling */
        if (fpiece==PIECE_T_LAST && gpiece==PIECE_T_LAST && hpiece==WHITE_ROOK 
            && !targets(state, BLACK, f1)){

            Move *move = create_move(e1, g1, WHITE, WHITE_KING, false, KINGSIDE);
            list_insert(moves, move);
        }

        /* Queenside castling */
        if (apiece==WHITE_ROOK && bpiece==PIECE_T_LAST && cpiece==PIECE_T_LAST 
            && dpiece==PIECE_T_LAST && !targets(state, BLACK, d1)){

            Move *move = create_move(e1, c1, WHITE, WHITE_KING, false, QUEENSIDE);
            list_insert(moves, move);
        }

    }
    else if (state->turn == BLACK && state->castling_rights[BLACK] && !in_check(state, BLACK)){

        assert(src == e8);

        piece_t apiece = get_piece_on_sqr(state, a8);
        piece_t bpiece = get_piece_on_sqr(state, b8);
        piece_t cpiece = get_piece_on_sqr(state, c8);
        piece_t dpiece = get_piece_on_sqr(state, d8);
        piece_t fpiece = get_piece_on_sqr(state, f8);
        piece_t gpiece = get_piece_on_sqr(state, g8);
        piece_t hpiece = get_piece_on_sqr(state, h8);
        
        /* Kingside castling */
        if (fpiece==PIECE_T_LAST && gpiece==PIECE_T_LAST && hpiece==BLACK_ROOK 
            && !targets(state, WHITE, f8)){

            Move *move = create_move(e8, g8, BLACK, BLACK_KING, false, KINGSIDE);
            list_insert(moves, move);
        }

        /* Queenside castling */
        if (apiece==BLACK_ROOK && bpiece==PIECE_T_LAST && cpiece==PIECE_T_LAST 
            && dpiece==PIECE_T_LAST && !targets(state, WHITE, d8)){
            
            Move *move = create_move(e8, c8, BLACK, BLACK_KING, false, QUEENSIDE);
            list_insert(moves, move);
        }
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
        make_move_on(move, &dummy_state);
        
        /* If after playing move player is in check remove */
        if (in_check(&dummy_state, state->turn))
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
    BB_t king_bb = state->piece_bbs[side==WHITE?WHITE_KING:BLACK_KING];
    sqr_t king = pop_lsb(&king_bb);
    return targets(state, side==WHITE?BLACK:WHITE, king);
}

bool in_checkmate(ChessState *state){
    List *moves = list_create(compare_moves, destroy_move);
    gen_legal_moves(state, moves);
    legalize_moves(state, moves);
    int n = moves->size;
    list_destroy(moves);
    return in_check(state, state->turn) && n == 0;
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


bool targets(ChessState *state, color_t turn, sqr_t sqr){
    
    /* Check if knight targets */
    BB_t knight_bb = (turn == WHITE) ?
        state->piece_bbs[WHITE_KNIGHT]:
        state->piece_bbs[BLACK_KNIGHT];
    if (knight_bb & attck_bbs.knight_attck[sqr])
        return true;

    /* Check if bishop or queen diagonally targets */
    piece_t bishop = (turn == WHITE) ?
        WHITE_BISHOP:BLACK_BISHOP;
    piece_t queen = (turn == WHITE) ?
        WHITE_QUEEN:BLACK_QUEEN;
    for (int i = 1; i < 8; i++){
        sqr_t src = north_west(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(state, src);
        if (p == bishop || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = north_east(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(state, src);
        if (p == bishop || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = south_west(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(state, src);
        if (p == bishop || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = south_east(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(state, src);
        if (p == bishop || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }

    /* Check if rook or queen horizontally targets */
    piece_t rook = (turn == WHITE) ?
        WHITE_ROOK:BLACK_ROOK;
    for (int i = 1; i < 8; i++){
        sqr_t src = north(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(state, src);
        if (p == rook || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = west(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(state, src);
        if (p == rook || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = east(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(state, src);
        if (p == rook || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = south(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(state, src);
        if (p == rook || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }


    /* Check if king targets */
    piece_t king = (turn == WHITE) ?
        WHITE_KING:BLACK_KING;
    sqr_t neighbours[MAX_NEIGHBOURS];
    get_neighbours(sqr, neighbours);
    for (int i = 0;i < MAX_NEIGHBOURS; i++){
        if (neighbours[i] != SQR_OUT && get_piece_on_sqr(state, neighbours[i]) == king)
            return true;
    }


    /* Check if pawn targets */
    if (turn == WHITE){
        sqr_t s1,s2;
        s1 = south_west(sqr, 1);
        s2 = south_east(sqr, 1);
        if (s1 != SQR_OUT && (get_piece_on_sqr(state, s1) == WHITE_PAWN))
            return true;
        if (s2 != SQR_OUT && (get_piece_on_sqr(state, s2) == WHITE_PAWN))
            return true;
    }
    if (turn == BLACK){
        sqr_t s1,s2;
        s1 = north_west(sqr, 1);
        s2 = north_east(sqr, 1);
        if (s1 != SQR_OUT && (get_piece_on_sqr(state, s1) == BLACK_PAWN))
            return true;
        if (s2 != SQR_OUT && (get_piece_on_sqr(state, s2) == BLACK_PAWN))
            return true;
    }

    return false;
}
