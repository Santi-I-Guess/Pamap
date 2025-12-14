#ifndef DEBUG_FUNCS_H
#define DEBUG_FUNCS_H 1

#include "generic_array.h"
#include "structure.h"
#include "types.h"

void print_coord(Coordinate coord);
void print_structure(Structure *alpha);
void print_tokens(ARRAY_NAME(Token) tokens);

#endif
