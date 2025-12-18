#include <stdio.h>

#include "includes/debug_funcs.h"
#include "includes/generic_array.h"

void print_coord(Coordinate coord) {
        printf("(%d, %d, %d)", coord.x, coord.y, coord.z);
}

void print_structure(Structure *alpha) {
        printf("--- EDGES ---\n");
        for (size_t i = 0; i < alpha->edge_count; i++) {
                Coordinate first = alpha->edges[i].first;
                Coordinate second = alpha->edges[i].second;
                char symbol = alpha->edges[i].symbol;
                printf("%c --- ", symbol);
                printf("(%d, %d, %d), ", first.x, first.y, first.z);
                printf("(%d, %d, %d)\n", second.x, second.y, second.z);
        }
        for (size_t i = 0; i < alpha->trails.count; i++) {
                printf("--- Trail %ld ---\n", i);
                Trail curr_trail = alpha->trails.data[i];
                for (size_t j = 0; j < curr_trail.count; j++) {
                        Coordinate curr_coord = curr_trail.data[j];
                        printf("-> (%d, %d, %d)\n",
                               curr_coord.x, curr_coord.y, curr_coord.z);
                }
        }
}

void print_tokens(ARRAY_NAME(Token) token_array)  {
        for (size_t i = 0; i < token_array.count; i++) {
                printf("\"%s\"\n", token_array.data[i].data);
        }
}
