#ifndef ERR_HANDLE_H
#define ERR_HANDLE_H

#include <stdio.h>

#define PSTAT(stat, label){                 \
    if (stat == STAT_NULLPTR){              \
        printf("%s: STAT_NULLPTR\n", label);  \
    }                                       \
    if (stat == STAT_ERR){                  \
        printf("%s: STAT_ERR\n", label);      \
    }                                       \
    if (stat == STAT_NOTFOUND){                  \
        printf("%s: STAT_NOTFOUND\n", label);      \
    }                                       \
    exit(stat);                             \
}                                           \


typedef enum {
    STAT_SUCCESS,
    STAT_NULLPTR,
    STAT_ERR,
    STAT_NOTFOUND
} status_t;

#endif