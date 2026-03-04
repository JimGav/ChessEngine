#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "chess.h"
#include "sqr.h"
#include "bitboard.h"
#include "err.h"


/* Global variables */
ChessState state;
attck_bbs_t attck_bbs;


bool make_move(Move *move){

    /* Backup state in case move is illegal */
    ChessState dstate = state;


    /* Update captured piece bb */
    piece_t captured_piece;
    sqr_t captured_sqr;
    if (move->is_ep){
        captured_piece = (move->side == WHITE) ? BLACK_PAWN : WHITE_PAWN;
        captured_sqr = (move->side == WHITE) ? south(move->target,1) : north(move->target,1);
    }
    else{
        captured_piece = get_piece_on_sqr(move->target);
        captured_sqr = move->target;
    }
    set_zero(&state.piece_bbs[captured_piece], captured_sqr);


    /* Update moving piece bb */
    set_zero(&state.piece_bbs[move->piece], move->origin);
    set_one(&state.piece_bbs[move->piece], move->target);


    /* If castling update rook bb as well */
    if (move->piece == WHITE_KING && move->castling != NO_CASTLING){
        sqr_t rook_src = move->castling == KINGSIDE ? h1 : a1;
        sqr_t rook_target = move->castling == KINGSIDE ? f1 : d1;
        set_zero(&state.piece_bbs[WHITE_ROOK], rook_src);
        set_one(&state.piece_bbs[WHITE_ROOK], rook_target);
    }
    if (move->piece == BLACK_KING && move->castling != NO_CASTLING){
        sqr_t rook_src = move->castling == KINGSIDE ? h8 : a8;
        sqr_t rook_target = move->castling == KINGSIDE ? f8 : d8;
        set_zero(&state.piece_bbs[BLACK_ROOK], rook_src);
        set_one(&state.piece_bbs[BLACK_ROOK], rook_target);
    }


    /* Update castling rights */ //todo: rook moves or captures
    if (move->piece == WHITE_KING || move->piece == BLACK_KING)
        state.castling_rights[move->side] = false;


    update_bbs();


    /* Update state.ep_target */
    if (double_pawn_move(move)){
        state.ep_target = (move->side == WHITE) ? 
            south(move->target, 1):
            north(move->target, 1);
    }
    else
        state.ep_target = SQR_OUT;
    
        
    /* Update turn */
    state.turn = (state.turn == WHITE) ? BLACK : WHITE;


    /* If move illegal restore last state */
    if (in_check(move->side)){
        state = dstate;
        return false;
    }
    
    
    return true;
}


status_t gen_start_state(){

    memset(&state, 0, sizeof(ChessState));
    
    BB_t *piece_bbs = state.piece_bbs;

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

    update_bbs();

    state.castling_rights[WHITE] = true;
    state.castling_rights[BLACK] = true;
    
    state.ep_target = SQR_OUT;
    
    state.turn = WHITE;

    return STAT_SUCCESS;
}


/* 
    Generates an attack bitboard for each piece type and stores it in bbs 
    bbs = bb array of array of bbs for each piece type
*/
status_t gen_attck_bbs(){ //todo: precompute ranks files diagonals

    for (sqr_t sqr = a1; sqr <= h8; sqr++){


        /* Pawn capture bb */
        attck_bbs.white_pawn_capt[sqr] = 0;
        attck_bbs.black_pawn_capt[sqr] = 0;
        sqr_t l,r;
        l = north_west(sqr,1);
        r = north_east(sqr,1);
        if (l != SQR_OUT) set_one(&attck_bbs.white_pawn_capt[sqr], l);
        if (r != SQR_OUT) set_one(&attck_bbs.white_pawn_capt[sqr], r);
        l = south_west(sqr,1);
        r = south_east(sqr,1);
        if (l != SQR_OUT) set_one(&attck_bbs.black_pawn_capt[sqr], l);
        if (r != SQR_OUT) set_one(&attck_bbs.black_pawn_capt[sqr], r);

        
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
status_t gen_moves(List *moves){

    gen_pawn_moves(moves);
    gen_knight_moves(moves);
    gen_bishop_moves(moves);
    gen_rook_moves(moves);
    gen_queen_moves(moves);
    gen_king_moves(moves);
    
    return STAT_SUCCESS;
}


status_t gen_pawn_moves(List *moves){

    BB_t rest_pawns = state.turn == WHITE ?
        state.piece_bbs[WHITE_PAWN]:
        state.piece_bbs[BLACK_PAWN];

    sqr_t src,target;
    while ((src = pop_lsb(&rest_pawns)) != -1){
        
        if (state.turn == WHITE){
            
            /* Single pawn push */
            target = north(src, 1);
            if (target == SQR_OUT)
                continue;

            Move *move;
            if (!(sqr_to_bb(target) & state.all_bb)){
                move = create_move(src, target, WHITE, WHITE_PAWN, PIECE_T_LAST, false, NO_CASTLING);
                list_insert(moves, move);
            }

            /* Double pawn push */
            sqr_t target2 = north(src, 2);
            if ((get_rank(src) == 1) && !(sqr_to_bb(target) & state.all_bb) && !(sqr_to_bb(target2) & state.all_bb)){
                move = create_move(src, target2, WHITE, WHITE_PAWN, PIECE_T_LAST, false, NO_CASTLING);
                list_insert(moves, move);
            }

            /* Captures */
            BB_t ep_target_bb = state.ep_target != SQR_OUT ? sqr_to_bb(state.ep_target) : 0;
            BB_t white_capt = attck_bbs.white_pawn_capt[src] & (state.black_bb | ep_target_bb);
            while ((target = pop_lsb(&white_capt)) != -1){
                piece_t captured = target == state.ep_target ? BLACK_PAWN : get_piece_on_sqr(target);
                move = create_move(src, target, WHITE, WHITE_PAWN, captured, target == state.ep_target, NO_CASTLING); 
                list_insert(moves, move);   
            }
        }
        else if (state.turn == BLACK){
            
            /* Single pawn push */
            target = south(src, 1);
            if (target == SQR_OUT)
                continue;

            Move *move;
            if (!(sqr_to_bb(target) & state.all_bb)){
                move = create_move(src, target, BLACK, BLACK_PAWN, PIECE_T_LAST, false, NO_CASTLING);
                list_insert(moves, move);
            }

            /* Double pawn push */
            sqr_t target2 = south(src, 2);
            if ((get_rank(src) == 6) && !(sqr_to_bb(target) & state.all_bb) && !(sqr_to_bb(target2) & state.all_bb)){
                move = create_move(src, target2, BLACK, BLACK_PAWN, PIECE_T_LAST, false, NO_CASTLING);
                list_insert(moves, move);
            }

            /* Captures */
            BB_t ep_target_bb = state.ep_target != SQR_OUT ? sqr_to_bb(state.ep_target) : 0;
            BB_t black_capt = attck_bbs.black_pawn_capt[src] & (state.white_bb | ep_target_bb);
            while ((target = pop_lsb(&black_capt)) != -1){
                piece_t captured = target == state.ep_target ? WHITE_PAWN : get_piece_on_sqr(target);
                move = create_move(src, target, BLACK, BLACK_PAWN, captured, target == state.ep_target, NO_CASTLING);  
                list_insert(moves, move);   
            }
        }
    }

    return STAT_SUCCESS;
}


status_t gen_knight_moves(List *moves){

    BB_t rest_knights = state.turn == WHITE ? 
        state.piece_bbs[WHITE_KNIGHT]: 
        state.piece_bbs[BLACK_KNIGHT];
    BB_t same_color_bb = state.turn == WHITE ?
        state.white_bb:
        state.black_bb;

    sqr_t src, target;
    while ((src = pop_lsb(&rest_knights)) != -1){
        BB_t rest_attcks = attck_bbs.knight_attck[src] & ~same_color_bb;
        while ((target = pop_lsb(&rest_attcks)) != -1){
            Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT, get_piece_on_sqr(target), false, NO_CASTLING);
            list_insert(moves, move);
        }
    }

    return STAT_SUCCESS;
}

status_t gen_bishop_moves(List *moves){

    BB_t rest_bishops = state.turn == WHITE ? 
        state.piece_bbs[WHITE_BISHOP]: 
        state.piece_bbs[BLACK_BISHOP];
    BB_t same_color_bb = state.turn == WHITE ?
        state.white_bb:
        state.black_bb;
    BB_t opp_color_bb = state.turn == WHITE ?
        state.black_bb:
        state.white_bb;


    /* Bishop moves */ // todo: OPtimize with attck bb
    sqr_t src, target;
    while ((src = pop_lsb(&rest_bishops)) != -1){
        
        for (int i = 1; i < 8; i++){
            target = north_east(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP, get_piece_on_sqr(target), false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }


        for (int i = 1; i < 8; i++){
            target = north_west(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP, get_piece_on_sqr(target), false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 8; i++){
            target = south_east(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP, get_piece_on_sqr(target), false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 8; i++){
            target = south_west(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_BISHOP : BLACK_BISHOP, get_piece_on_sqr(target), false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }
    }    

    return STAT_SUCCESS;
}

status_t gen_rook_moves(List *moves){

    BB_t rest_rooks = state.turn == WHITE ? 
        state.piece_bbs[WHITE_ROOK]: 
        state.piece_bbs[BLACK_ROOK];
    BB_t same_color_bb = state.turn == WHITE ?
        state.white_bb:
        state.black_bb;
    BB_t opp_color_bb = state.turn == WHITE ?
        state.black_bb:
        state.white_bb;


    /* Rook moves */ // todo: OPtimize with attck bb
    sqr_t src, target;
    while ((src = pop_lsb(&rest_rooks)) != -1){
        
        for (int i = 1; i < 8; i++){
            target = north(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_ROOK : BLACK_ROOK, get_piece_on_sqr(target), false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }


        for (int i = 1; i < 8; i++){
            target = west(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_ROOK : BLACK_ROOK, get_piece_on_sqr(target), false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 8; i++){
            target = south(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_ROOK : BLACK_ROOK, get_piece_on_sqr(target), false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }

        for (int i = 1; i < 8; i++){
            target = east(src, i);
            if (target == SQR_OUT || (sqr_to_bb(target) & same_color_bb))
                break;
            
            Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_ROOK : BLACK_ROOK, get_piece_on_sqr(target), false, NO_CASTLING);
            list_insert(moves, move);

            if (sqr_to_bb(target) & opp_color_bb)
                break;
        }
    }    

    return STAT_SUCCESS;
}

status_t gen_queen_moves(List *moves){

    BB_t same_color_bb = state.turn == WHITE ?
        state.white_bb:
        state.black_bb;
    BB_t opp_color_bb = state.turn == WHITE ?
        state.black_bb:
        state.white_bb;

    BB_t rest_queens = state.turn == WHITE ?
        state.piece_bbs[WHITE_QUEEN]:
        state.piece_bbs[BLACK_QUEEN];

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
        Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, get_piece_on_sqr(target), false, NO_CASTLING);
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
        Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, get_piece_on_sqr(target), false, NO_CASTLING);
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
        Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, get_piece_on_sqr(target), false, NO_CASTLING);
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
        Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, get_piece_on_sqr(target), false, NO_CASTLING);
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
        Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, get_piece_on_sqr(target), false, NO_CASTLING);
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
        Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, get_piece_on_sqr(target), false, NO_CASTLING);
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
        Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, get_piece_on_sqr(target), false, NO_CASTLING);
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
        Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_QUEEN : BLACK_QUEEN, get_piece_on_sqr(target), false, NO_CASTLING);
        list_insert(moves, move);

        if (sqr_to_bb(target) & opp_color_bb)
            break;
    }

    return STAT_SUCCESS;
}

status_t gen_king_moves(List *moves){

    BB_t king_bb = state.turn == WHITE ?
        state.piece_bbs[WHITE_KING] :
        state.piece_bbs[BLACK_KING];
    BB_t same_color_bb = state.turn == WHITE ?
        state.white_bb:
        state.black_bb;
        
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
        Move *move = create_move(src, target, state.turn, state.turn == WHITE ? WHITE_KING : BLACK_KING, get_piece_on_sqr(target), false, NO_CASTLING);
        list_insert(moves, move);
    } 
    
    /* Castling */
    if (state.turn == WHITE && state.castling_rights[WHITE] && !in_check(WHITE)){

        assert(src == e1);

        piece_t apiece = get_piece_on_sqr(a1);
        piece_t bpiece = get_piece_on_sqr(b1);
        piece_t cpiece = get_piece_on_sqr(c1);
        piece_t dpiece = get_piece_on_sqr(d1);
        piece_t fpiece = get_piece_on_sqr(f1);
        piece_t gpiece = get_piece_on_sqr(g1);
        piece_t hpiece = get_piece_on_sqr(h1);

        /* Kingside castling */
        if (fpiece==PIECE_T_LAST && gpiece==PIECE_T_LAST && hpiece==WHITE_ROOK 
            && !targets(BLACK, f1)){

            Move *move = create_move(e1, g1, WHITE, WHITE_KING, PIECE_T_LAST, false, KINGSIDE);
            list_insert(moves, move);
        }

        /* Queenside castling */
        if (apiece==WHITE_ROOK && bpiece==PIECE_T_LAST && cpiece==PIECE_T_LAST 
            && dpiece==PIECE_T_LAST && !targets(BLACK, d1)){

            Move *move = create_move(e1, c1, WHITE, WHITE_KING, PIECE_T_LAST, false, QUEENSIDE);
            list_insert(moves, move);
        }

    }
    else if (state.turn == BLACK && state.castling_rights[BLACK] && !in_check(BLACK)){

        assert(src == e8);

        piece_t apiece = get_piece_on_sqr(a8);
        piece_t bpiece = get_piece_on_sqr(b8);
        piece_t cpiece = get_piece_on_sqr(c8);
        piece_t dpiece = get_piece_on_sqr(d8);
        piece_t fpiece = get_piece_on_sqr(f8);
        piece_t gpiece = get_piece_on_sqr(g8);
        piece_t hpiece = get_piece_on_sqr(h8);
        
        /* Kingside castling */
        if (fpiece==PIECE_T_LAST && gpiece==PIECE_T_LAST && hpiece==BLACK_ROOK 
            && !targets(WHITE, f8)){

            Move *move = create_move(e8, g8, BLACK, BLACK_KING, PIECE_T_LAST, false, KINGSIDE);
            list_insert(moves, move);
        }

        /* Queenside castling */
        if (apiece==BLACK_ROOK && bpiece==PIECE_T_LAST && cpiece==PIECE_T_LAST 
            && dpiece==PIECE_T_LAST && !targets(WHITE, d8)){
            
            Move *move = create_move(e8, c8, BLACK, BLACK_KING, PIECE_T_LAST, false, QUEENSIDE);
            list_insert(moves, move);
        }
    }
    
    return STAT_SUCCESS;
}


status_t update_bbs(){
    state.white_bb = state.piece_bbs[WHITE_PAWN]   | state.piece_bbs[WHITE_KNIGHT] 
                    | state.piece_bbs[WHITE_BISHOP] | state.piece_bbs[WHITE_ROOK] 
                    | state.piece_bbs[WHITE_KING]   | state.piece_bbs[WHITE_QUEEN];

    state.black_bb = state.piece_bbs[BLACK_PAWN]   | state.piece_bbs[BLACK_KNIGHT] 
                    | state.piece_bbs[BLACK_BISHOP] | state.piece_bbs[BLACK_ROOK] 
                    | state.piece_bbs[BLACK_KING]   | state.piece_bbs[BLACK_QUEEN];

    state.all_bb = state.white_bb | state.black_bb;
    
    return STAT_SUCCESS;
}

bool in_check(color_t side){
    BB_t king_bb = state.piece_bbs[side==WHITE?WHITE_KING:BLACK_KING];
    sqr_t king = pop_lsb(&king_bb);
    return targets(side==WHITE?BLACK:WHITE, king);
}

bool in_checkmate(){
    List *moves = list_create(compare_moves, destroy_move);
    gen_moves(moves);
    
    int n = 0;
    ListNode *node = moves->head;
    while (node){
        Move *move = node->dt_ptr;

        ChessState dstate = state;
        if (make_move(move)){
            if (!in_check(move->side)) n++;
            state = dstate;
        }

        node = node->next;
    }

    list_destroy(moves);
    return in_check(state.turn) && n == 0;
}


piece_t get_piece_on_sqr(sqr_t sqr){
    
    BB_t sqr_bb = sqr_to_bb(sqr);
    for (piece_t i = 0; i < PIECE_T_LAST; i++){
        if (state.piece_bbs[i] & sqr_bb)
            return i;
    }

    return PIECE_T_LAST;
}


bool targets(color_t turn, sqr_t sqr){
    
    /* Check if knight targets */
    BB_t knight_bb = (turn == WHITE) ?
        state.piece_bbs[WHITE_KNIGHT]:
        state.piece_bbs[BLACK_KNIGHT];
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
        piece_t p = get_piece_on_sqr(src);
        if (p == bishop || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = north_east(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(src);
        if (p == bishop || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = south_west(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(src);
        if (p == bishop || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = south_east(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(src);
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
        piece_t p = get_piece_on_sqr(src);
        if (p == rook || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = west(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(src);
        if (p == rook || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = east(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(src);
        if (p == rook || p == queen)
            return true;
        else if (p != PIECE_T_LAST)
            break;
    }
    for (int i = 1; i < 8; i++){
        sqr_t src = south(sqr, i);
        if (src == SQR_OUT)
            break;
        piece_t p = get_piece_on_sqr(src);
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
        if (neighbours[i] != SQR_OUT && get_piece_on_sqr(neighbours[i]) == king)
            return true;
    }


    /* Check if pawn targets */ //todo: ep
    piece_t pawns = (turn == WHITE) ? WHITE_PAWN : BLACK_PAWN;
    if (attck_bbs.black_pawn_capt[sqr] & state.piece_bbs[pawns])
        return true;

    return false;
}
