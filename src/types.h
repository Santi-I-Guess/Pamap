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

DEFINE_ARRAY_STRUCT(Token)

typedef struct {
        int x, y, z;
} Coordinate;

typedef struct {
        int x1, y1, z1;
        int x2, y2, z2;
} Edge;

typedef struct {
        size_t coord_count;
        size_t coord_capacity;
        Coordinate *coordinates;
        // where the index is calculated from the symbol ascii value
        // lower 26 is uppercase, higher 26 is lowercase
        size_t edge_count;
        Edge edges[52];
} Trail;

typedef struct {
        size_t count;
        size_t capacity;
        Trail *trails;
} Structure;

DECLARE_FREE_FUNC(Token)
DECLARE_INIT_FUNC(Token)
DECLARE_APPEND_FUNC(Token)

#endif
