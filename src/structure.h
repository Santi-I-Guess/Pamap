#ifndef STRUCTURE_H
#define STRUCTURE_H 1

#include <stdbool.h>
#include <stddef.h>

#include "generic_array.h"
#include "parser/tokenizer.h"

typedef struct {
        int x, y, z;
} Coordinate;

typedef struct {
        int x1, y1, z1;
        int x2, y2, z2;
} Edge;

DEFINE_ARRAY_STRUCT(Coordinate)
DECLARE_FREE_FUNC(Coordinate)
DECLARE_INIT_FUNC(Coordinate)
DECLARE_APPEND_FUNC(Coordinate)

typedef struct {
        ARRAY_NAME(Coordinate) coordinates;
} Trail;

DEFINE_ARRAY_STRUCT(Trail)
DECLARE_FREE_FUNC(Trail)
DECLARE_INIT_FUNC(Trail)
DECLARE_APPEND_FUNC(Trail)

typedef struct {
        ARRAY_NAME(Trail) trails;
        size_t edge_count; // can be a static array
        Edge edges[52]; // idx calculated with ascii value of symbol
} Structure;

#endif
