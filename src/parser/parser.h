#ifndef PARSER_H
#define PARSER_H 1

#include "../generic_array.h"
#include "../structure.h"

// populate Structure struct from tokens
Retval populate_structure(
        Structure *structure,
        const ARRAY_NAME(Token) *tokens
);

#endif
