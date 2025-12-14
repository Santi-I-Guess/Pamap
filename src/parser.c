#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "includes/generic_array.h"
#include "includes/structure.h"
#include "includes/parser.h"

#ifdef DEBUG
#include "includes/debug_funcs.h"
#endif

Retval handle_macro_token(
        Structure *structure,
        const Token curr_token,
        const Token next_token
) {
        // handle macro
        if (strcmp(curr_token.data, "#node_color") == 0) {
                // set node color of current trail
                if (strlen(next_token.data) != 6)
                        return MACRO_ARG_MALFORM;
                // take in hex input without prefix
                char red[3] = "", green[3] = "", blue[3] = "";
                strncpy(red, next_token.data, 2);
                strncpy(green, next_token.data + 2, 2);
                strncpy(blue, next_token.data + 4, 2);
                Trail *curr_trail = &structure->trails.data[
                        structure->trails.count-1];
                curr_trail->node_red   = (uint8_t)strtol(red, NULL, 16);
                curr_trail->node_green = (uint8_t)strtol(green, NULL, 16);
                curr_trail->node_blue  = (uint8_t)strtol(blue, NULL, 16);
        } else if (strcmp(curr_token.data, "#edge_color") == 0) {
                // set edge color of current trail
                if (strlen(next_token.data) != 6)
                        return MACRO_ARG_MALFORM;
                // take in hex input without prefix
                char red[3] = "", green[3] = "", blue[3] = "";
                strncpy(red, next_token.data, 2);
                strncpy(green, next_token.data + 2, 2);
                strncpy(blue, next_token.data + 4, 2);
                Trail *curr_trail = &structure->trails.data[
                        structure->trails.count-1];
                curr_trail->edge_red   = (uint8_t)strtol(red, NULL, 16);
                curr_trail->edge_green = (uint8_t)strtol(green, NULL, 16);
                curr_trail->edge_blue  = (uint8_t)strtol(blue, NULL, 16);
        } else if (strcmp(curr_token.data, "#start_pos") == 0) {
                int x, y, z;
                if (sscanf(next_token.data, "%d,%d,%d", &x, &y, &z) < 3) {
                        return MACRO_ARG_MALFORM;
                }
                structure->start_x = x;
                structure->start_y = y;
                structure->start_z = z;
        } else if (strcmp(curr_token.data, "#teleport_pos") == 0) {
                int x, y, z;
                if (sscanf(next_token.data, "%d,%d,%d", &x, &y, &z) < 3) {
                        return MACRO_ARG_MALFORM;
                }
                structure->tele_x = x;
                structure->tele_y = y;
                structure->tele_z = z;

        } else {
                return UNKNOWN_MACRO;
        }
        return GOOD;
}

Retval handle_normal_token(
        Structure *structure,
        Edge_Stacks *state,
        const Token token
) {
        // auxiliary vars
        bool (*p_fill_states)[52]     = &state->fill_states;
        bool (*p_pending_indices)[52] = &state->pending_indices;
        Coordinate (*p_data)[52][2]   = &state->data;
        size_t *p_edge_count = &structure->edge_count;
        Edge (*p_edges)[52]  = &structure->edges;

        if (token.data[0] == '[') {
                // start a new trail
                if (!alloc_trail_structure(structure))
                        return MEMORY_ERROR;
        } else if (token.data[0] == ']') {
        } else if (token.data[0] == '(') {
                // push coordinate to current trail
                Coordinate curr_coord;
                if (sscanf(token.data, "(%d,%d,%d)",
                           &curr_coord.x,
                           &curr_coord.y,
                           &curr_coord.z) < 3) {
                        return COORDINATE_MALFORM;
                }
                // push coordinate to current trail
                if (!append_coord_to_structure(structure, curr_coord))
                        return MEMORY_ERROR;

                // if any pending edges, add coord data to edge
                for (int i = 0; i < 52; i++) {
                        if (!(*p_pending_indices[i]))
                                continue;
                        if (*p_fill_states[i] == false) {
                                *p_data[i][0] = curr_coord;
                                *p_fill_states[i] = true;
                        } else {
                                *p_data[i][1] = curr_coord;
                                *p_fill_states[i] = false;
                                Edge next_edge;
                                next_edge.first = *p_data[i][0];
                                next_edge.second = *p_data[i][1];
                                next_edge.symbol = get_idx_char(i);
                                // push edge to struct
                                *p_edges[*p_edge_count] = next_edge;
                                (*p_edge_count)++;
                        }
                }

                memset(state->pending_indices, false, 52 * sizeof(bool));
        } else if (token.data[0] == '{') {
                // set edges active
                for (size_t i = 1; i < strlen(token.data); i++) {
                        int idx = get_edge_idx(token.data[i]);
                        if (idx != -1)
                                state->pending_indices[idx] = true;
                }
        }
        return GOOD;
}

Retval populate_structure(
        Structure *structure,
        const ARRAY_NAME(Token) *token_array
) {
        Edge_Stacks state = {0};
        memset(state.fill_states,
               false,
               sizeof(state.fill_states));

        // the parser should be pretty lenient: I don't want a repeat of the
        // PAL assembler lexing and grammar checking
        size_t tok_count = token_array->count;
        for (size_t token_idx = 0; token_idx < tok_count; ++token_idx) {
                Token curr_token = token_array->data[token_idx];
                Token next_token = {0};
                Retval token_res = GOOD;
                switch(curr_token.data[0]) {
                case '[':
                case ']':
                case '(':
                case '{':
                        token_res = handle_normal_token(
                                structure,
                                &state,
                                curr_token
                        );
                        break;
                case '#':
                        if (token_idx == token_array->count)
                                return EXPECTED_MACRO_ARG;
                        next_token = token_array->data[token_idx + 1];
                        token_res = handle_macro_token(
                                structure,
                                curr_token,
                                next_token
                        );
                        break;
                default:
                        // ignore macro arguments: those are handled by # case
                        break;
                }
                if (token_res != GOOD)
                        return token_res;
        }

        // push all non-trail edges at once

        return GOOD;
}
