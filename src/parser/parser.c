#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../generic_array.h"
#include "../structure.h"
#include "parser.h"

#ifdef DEBUG
#include "../debug_funcs.h"
#endif

typedef enum {
        E_AWAIT_FIRST,
        E_AWAIT_SECOND,
} Edge_State;

void generate_structure(Structure *structure, char **argv) {
        // store file in string
        FILE *source_file = fopen(argv[1], "r");
        if (!source_file) {
                fprintf(stderr, "failed to open file\n");
                exit(1);
        }
        Big_Str source = { 0 };
        store_input(source_file, &source);
        fclose(source_file);

        ARRAY_NAME(Token) token_array = {0};
        if (!INIT_FUNC(Token, &token_array, 128)) {
                fprintf(stderr, "failed to init tarray\n");
                free(source.data);
                exit(1);
        }
        Debug_Info context = tokenize_buffer(&source, &token_array);
        switch (context.retval) {
        case GOOD:
                break;
        case COORDINATE_MALFORM:  // not used in tokenize_buffer
        case EXPECTED_MACRO_ARG:  // not used in tokenize_buffer
        case MACRO_ARG_MALFORM:   // not used in tokenize_buffer
        case TRAIL_BRACE_MALFORM: // not used in tokenize_buffer
        case UNKNOWN_MACRO: // not used in tokenize_buffer
                break;
        case MEMORY_ERROR:
                printf("Error: memory error on line %d\n", context.line_num);
                free(source.data);
                FREE_FUNC(Token, &token_array);
                exit(0);
        case UNKNOWN_SYMBOL:
                printf("Error: unknown symbol \"%c\" on line %d\n", 
                       context.data[0], context.line_num);
                free(source.data);
                FREE_FUNC(Token, &token_array);
                exit(0);
        }
        free(source.data);

        // store into structure
        init_structure(structure);
        Retval populate_retval = populate_structure(structure, &token_array);
        switch (populate_retval) {
        case GOOD:
                break;
        case COORDINATE_MALFORM:
                printf("Error: coordinate malform in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case EXPECTED_MACRO_ARG:
                printf("Error: expected macro arg in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case MACRO_ARG_MALFORM:
                printf("Error: macro arg malform in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case MEMORY_ERROR:
                printf("Error: memory error in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case TRAIL_BRACE_MALFORM:
                printf("Error: trail brace malform in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case UNKNOWN_MACRO:
                printf("Error: unknown macro in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case UNKNOWN_SYMBOL: // not used in populate_structure
                break;
        }

#ifdef DEBUG
        print_tokens(token_array);
#endif

        FREE_FUNC(Token, &token_array);
}

Retval populate_structure(
        Structure *structure,
        const ARRAY_NAME(Token) *token_array
) {
        bool edge_operate_next[52]    = {0};
        Coordinate edge_stacks[52][2] = {0};
        Edge_State edge_stack_statuses[52];
        memset(edge_stack_statuses, E_AWAIT_FIRST, sizeof(edge_stack_statuses));

        // the parser should be pretty lenient: I don't want a repeat of the
        // PAL assembler lexing and grammar checking
        for (size_t token_idx = 0; token_idx < token_array->count; ++token_idx) {
                Token curr_token = token_array->data[token_idx];
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
                        for (size_t i = 1; i < strlen(curr_token.data); i++) {
                                int idx = get_edge_idx(curr_token.data[i]);
                                if (idx != -1)
                                        edge_operate_next[idx] = true;
                        }
                } else if (curr_token.data[0] == '#') {
                        // handle macro
                        if (token_idx == token_array->count - 1) {
                                return EXPECTED_MACRO_ARG;
                        } else if (strcmp(curr_token.data, "#node_color") == 0) {
                                // set node color of current trail
                                Token next_token = token_array->data[token_idx + 1];
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
                                Token next_token = token_array->data[token_idx + 1];
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
                                Token next_token = token_array->data[token_idx + 1];
                                int x, y, z;
                                if (sscanf(next_token.data, "%d,%d,%d", &x, &y, &z) < 3) {
                                        return MACRO_ARG_MALFORM;
                                }
                                structure->start_x = x;
                                structure->start_y = y;
                                structure->start_z = z;
                        } else if (strcmp(curr_token.data, "#teleport_pos") == 0) {
                                Token next_token = token_array->data[token_idx + 1];
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
                }
        }
        return GOOD;
}
