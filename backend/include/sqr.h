#ifndef SQR_H
#define SQR_H

#define VALID_SQR(sqr) (sqr >= a1 && sqr <= h8)

#define N(sqr, i) {\
  if (RANK(sqr) > 8-i)\
    return -1;\
  else\
    return (sqr+8*i);\
}\

#define W(sqr, j) {\
  if (FILE(sqr) < 1+j)\
    return -1;\
  else\
    return (sqr-j);\
}\

#define E(sqr, j) {\
  if (FILE(sqr) > 8-j)\
    return -1;\
  else\
    return (sqr+j);\
}\

#define S(sqr, i) {\
  if (RANK(sqr) < 1+i)\
    return -1;\
  else\
    return (sqr-8*i);\
}\

#define NW(sqr, i) {\
  if (N(sqr,i) == -1)\
    return -1\
  return W(N(sqr,i),i);\
}\

#define NE(sqr, i) {\
  if (N(sqr,i) == -1)\
    return -1\
  return E(N(sqr,i),i);\
}\

#define SW(sqr, i) {\
  if (S(sqr,i) == -1)\
    return -1\
  return W(S(sqr,i),i);\
}\

#define SE(sqr, i) {\
  if (S(sqr,i) == -1)\
    return -1\
  return E(S(sqr,i),i);\
}\

#define RANK(sqr) (sqr % 8)
#define FILE(sqr) (sqr / 8)

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