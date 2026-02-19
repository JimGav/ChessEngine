#ifndef SQR_H
#define SQR_H

#include <stdbool.h>

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


#endif