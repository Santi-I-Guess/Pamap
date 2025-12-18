#ifndef PARSER_H
#define PARSER_H 1

#include "generic_array.h"
#include "graphics.h"
#include "structure.h"

typedef struct {
        bool fill_states[52]; // (awaiting_first|awaiting_second)
        bool pending_indices[52];
        Coordinate data[52][2];
} Edge_Stacks;

Retval handle_macro_token(
        Structure *structure,
        Settings *settings,
        const Token curr_token,
        const Token next_token
);


Retval handle_normal_token(
        Structure *structure,
        Edge_Stacks *state,
        const Token token
);

// populate Structure struct from tokens
Retval populate_structure(
        Structure *structure,
        Settings *settings,
        const ARRAY_NAME(Token) *tokens
);

#endif
