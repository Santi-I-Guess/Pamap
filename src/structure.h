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
        Coordinate first;
        Coordinate second;
        char symbol;
} Edge;

DEFINE_ARRAY_STRUCT(Coordinate)
DECLARE_FREE_FUNC(Coordinate)
DECLARE_INIT_FUNC(Coordinate)
DECLARE_APPEND_FUNC(Coordinate)

typedef struct {
        size_t count;
        Coordinate data[64];
} Trail;


DEFINE_ARRAY_STRUCT(Trail)
DECLARE_FREE_FUNC(Trail)
DECLARE_INIT_FUNC(Trail)
DECLARE_APPEND_FUNC(Trail)

typedef struct {
        ARRAY_NAME(Trail) trails; // 2d array of coordinates
        size_t edge_count;
        Edge edges[52];
} Structure;

bool init_structure(Structure *alpha);
void free_structure(Structure *alpha);

// allocate a new trail for structure
bool alloc_trail_structure(Structure *alpha);

// will append a coordinate to highest allocated trail
bool append_coord_to_structure(Structure *alpha, Coordinate beta);

int get_edge_idx(const char i);
char get_idx_char(const int idx);

#endif
