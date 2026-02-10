#ifndef SQR_H
#define SQR_H

#define BOARD_RANK(sqr) (sqr % 8)
#define BOARD_FILE(sqr) (sqr / 8)

#define VALID_SQR(sqr) (sqr >= a1 && sqr <= h8)

#define N(sqr, i) (BOARD_RANK(sqr) > 8-i ? (-1) : (sqr+8*i))
#define W(sqr, j) (BOARD_FILE(sqr) < 1+j ? (-1) : (sqr-j))
#define E(sqr, j) (BOARD_FILE(sqr) > 8-j ? (-1) : (sqr+j))
#define S(sqr, i) (BOARD_RANK(sqr) < 1+i ? (-1) : (sqr-8*i))

#define NW(sqr, i) (N(sqr,i) == -1 ? -1 : W(N(sqr,i),i))
#define NE(sqr, i) (N(sqr,i) == -1 ? -1 : E(N(sqr,i),i))
#define SW(sqr, i) (S(sqr,i) == -1 ? -1 : W(S(sqr,i),i))
#define SE(sqr, i) (S(sqr,i) == -1 ? -1 : E(S(sqr,i),i))


typedef enum {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8
} sqr_t;

#endif