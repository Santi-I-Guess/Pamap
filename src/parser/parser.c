#include <stdio.h>
#include <string.h>

#include "../generic_array.h"
#include "../structure.h"
#include "parser.h"

typedef enum {
        E_AWAIT_FIRST,
        E_AWAIT_SECOND,
} Edge_State;

Retval populate_structure(
        Structure *structure,
        const ARRAY_NAME(Token) *tokens
) {
        bool edge_operate_next[52]    = {0};
        Coordinate edge_stacks[52][2] = {0};
        Edge_State edge_stack_statuses[52];
        memset(edge_stack_statuses, E_AWAIT_FIRST, sizeof(edge_stack_statuses));

        // the parser should be pretty lenient: I don't want a repeat of the
        // PAL assembler lexing and grammar checking
        for (size_t i = 0; i < tokens->count; ++i) {
                Token curr_token = tokens->data[i];
                if (curr_token.data[0] == '[') {
                        // start a new trail
                        if (!alloc_trail_structure(structure))
                                return MEMORY_ERROR;
                } else if (curr_token.data[0] == ']') {
                } else if (curr_token.data[0] == '(') {
                        // push coordinate to current trail
                        Coordinate curr_coord;
                        if (sscanf(curr_token.data, "(%d,%d,%d)",
                                   &curr_coord.x, &curr_coord.y, &curr_coord.z) < 3) {
                                return COORDINATE_MALFORM;
                        }
                        // push coordinate to current trail
                        if (!append_coord_to_structure(structure, curr_coord))
                                return MEMORY_ERROR;

                        // if any pending edges, add coord data to edge
                        for (int i = 0; i < 52; i++) {
                                if (!edge_operate_next[i])
                                        continue;
                                if (edge_stack_statuses[i] == E_AWAIT_FIRST) {
                                        edge_stacks[i][0] = curr_coord;
                                        edge_stack_statuses[i] = E_AWAIT_SECOND;
                                } else {
                                        edge_stacks[i][1] = curr_coord;
                                        edge_stack_statuses[i] = E_AWAIT_FIRST;
                                        Edge next_edge;
                                        next_edge.first = edge_stacks[i][0];
                                        next_edge.second = edge_stacks[i][1];
                                        next_edge.symbol = get_idx_char(i);
                                        structure->edges[structure->edge_count] = next_edge;
                                        (structure->edge_count)++;
                                }
                        }

                        memset(edge_operate_next, false, 52 * sizeof(bool));
                } else if (curr_token.data[0] == '{') {
                        // set edges active
                        for (size_t i = 0; i < strlen(curr_token.data); i++) {
                                int idx = get_edge_idx(curr_token.data[i]);
                                if (idx != -1)
                                        edge_operate_next[idx] = true;
                        }
                }
        }
        return GOOD;
}
