#ifndef TYPES_H
#define TYPES_H 1

#include <stdbool.h>
#include <stddef.h>

#include "generic_array.h"

typedef struct {
        size_t length;
        char *data;
} Big_Str;

typedef struct {
        char data[64];
        int line_num;
} Token;

typedef enum {
        GOOD,
        MEMORY_ERROR,
        COORDINATE_MALFORM,
        TRAIL_BRACE_MALFORM,
        UNKNOWN_SYMBOL,
} Retval;

DEFINE_ARRAY_STRUCT(Token)
DECLARE_FREE_FUNC(Token)
DECLARE_INIT_FUNC(Token)
DECLARE_APPEND_FUNC(Token)

#endif
