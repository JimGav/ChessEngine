#ifndef SQR_H
#define SQR_H

#include <stdbool.h>


#define MAX_NEIGHBOURS 8


typedef enum {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
    SQR_OUT
} sqr_t;


typedef enum {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
} rank_t;


typedef enum {
    FILE_1, FILE_2, FILE_3, FILE_4, FILE_5, FILE_6, FILE_7, FILE_8
} file_t;


#define RANK_1_BB ((BB_t)(0x00000000000000FF))
#define RANK_2_BB ((BB_t)(0x000000000000FF00))
#define RANK_3_BB ((BB_t)(0x0000000000FF0000))
#define RANK_4_BB ((BB_t)(0x00000000FF000000))
#define RANK_5_BB ((BB_t)(0x000000FF00000000))
#define RANK_6_BB ((BB_t)(0x0000FF0000000000))
#define RANK_7_BB ((BB_t)(0x00FF000000000000))
#define RANK_8_BB ((BB_t)(0xFF00000000000000))

#define FILE_1_BB ((BB_t)(0x0101010101010101))
#define FILE_2_BB ((BB_t)(0x0202020202020202))
#define FILE_3_BB ((BB_t)(0x0404040404040404))
#define FILE_4_BB ((BB_t)(0x0808080808080808))
#define FILE_5_BB ((BB_t)(0x1010101010101010))
#define FILE_6_BB ((BB_t)(0x2020202020202020))
#define FILE_7_BB ((BB_t)(0x4040404040404040))
#define FILE_8_BB ((BB_t)(0x8080808080808080))


bool valid_sqr(sqr_t sqr);

file_t get_rank(sqr_t sqr);
file_t get_file(sqr_t sqr);

/* Compass functions */
sqr_t north(sqr_t sqr, int i);
sqr_t south(sqr_t sqr, int i);
sqr_t east(sqr_t sqr, int j);
sqr_t west(sqr_t sqr, int j);
sqr_t north_west(sqr_t sqr, int i);
sqr_t north_east(sqr_t sqr, int i);
sqr_t south_west(sqr_t sqr, int i);
sqr_t south_east(sqr_t sqr, int i);

void get_neighbours(sqr_t sqr, sqr_t *neighbours);

#endif