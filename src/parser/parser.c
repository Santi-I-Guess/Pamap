#include <stdio.h>
#include <string.h>

#include "../generic_array.h"
#include "../structure.h"
#include "parser.h"

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
        case COORDINATE_MALFORM: // not used in tokenize_buffer
        case TRAIL_BRACE_MALFORM: // not used in tokenize_buffer
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
        case UNKNOWN_SYMBOL: // not used in populate_structure
                break;
        case COORDINATE_MALFORM:
                printf("Error: coordinate malform in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case TRAIL_BRACE_MALFORM:
                printf("Error: trail brace malform in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case MEMORY_ERROR:
                printf("Error: memory error in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        }
        FREE_FUNC(Token, &token_array);
}

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
