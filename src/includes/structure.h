#ifndef STRUCTURE_H
#define STRUCTURE_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "generic_array.h"
#include "tokenizer.h"

typedef struct {
        int x, y, z;
} Coordinate;

typedef struct {
        Coordinate first;
        Coordinate second;
        char symbol;
} Edge;

DEFINE_ARRAY_STRUCT(Coordinate)
DECLARE_FREE_FUNC(Coordinate);
DECLARE_INIT_FUNC(Coordinate);
DECLARE_APPEND_FUNC(Coordinate);

typedef struct {
        size_t count;
        Coordinate data[64];
        uint8_t node_red, node_green, node_blue;
        uint8_t edge_red, edge_green, edge_blue;
} Trail;

DEFINE_ARRAY_STRUCT(Trail)
DECLARE_FREE_FUNC(Trail);
DECLARE_INIT_FUNC(Trail);
DECLARE_APPEND_FUNC(Trail);

typedef struct {
        ARRAY_NAME(Trail) trails; // 2d array of coordinates
        size_t edge_count;
        Edge edges[52];
        bool unique_start_view;
        int start_x, start_y, start_z;
        int start_view_x, start_view_y, start_view_z;
        int tele_x, tele_y, tele_z;
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
