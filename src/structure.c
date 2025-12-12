#include "structure.h"

DEFINE_FREE_FUNC(Coordinate)
DEFINE_INIT_FUNC(Coordinate)
DEFINE_APPEND_FUNC(Coordinate)

DEFINE_FREE_FUNC(Trail)
DEFINE_INIT_FUNC(Trail)
DEFINE_APPEND_FUNC(Trail)

bool init_structure(Structure *alpha) {
        alpha->start_x = 10;
        alpha->start_y = 10;
        alpha->start_z = 10;
        return INIT_FUNC(Trail, &alpha->trails, 8);
}

void free_structure(Structure *alpha) {
        FREE_FUNC(Trail, &alpha->trails);
}

int get_edge_idx(const char i) {
        if ('A' <= i && i <= 'Z')
                return (int)(i - 'A');
        else if ('a' <= i && i <= 'z')
                return (int)(i - 'a') + 26;
        return -1;
}

char get_idx_char(const int idx) {
        if (idx < 26)
                return 'A' + (char)idx;
        else if (idx < 52)
                return 'a' + (char)(idx - 26);
        return -1;
}


bool alloc_trail_structure(Structure *alpha) {
        Trail curr_trail = {0};
        // default colors: black nodes, gray edges
        curr_trail.node_red   = 0x00;
        curr_trail.node_green = 0x00;
        curr_trail.node_blue  = 0x00;
        curr_trail.edge_red   = 0xaa;
        curr_trail.edge_green = 0xaa;
        curr_trail.edge_blue  = 0xaa;
        return APPEND_FUNC(Trail, &(alpha->trails), curr_trail);
}

bool append_coord_to_structure(Structure *alpha, Coordinate beta) {
        Trail *curr_trail = &(alpha->trails.data[alpha->trails.count - 1]);
        if (curr_trail->count == 64)
                return false;
        curr_trail->data[curr_trail->count] = beta;
        (curr_trail->count)++;
        return true;
}
