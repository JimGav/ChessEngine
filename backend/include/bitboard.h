#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "sqr.h"
#include "err_handle.h"

#define BB_t uint64_t

status_t set_one(BB_t *bb, sqr_t sqr);
status_t set_zero(BB_t *bb, sqr_t sqr);

#endif