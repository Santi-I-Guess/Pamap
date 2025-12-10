#ifndef TOKENIZER_H
#define TOKENIZER_H 1

#include "generic_array.h"
#include "types.h"

typedef enum {
        GOOD,
        MEMORY_ERROR,
        UNKNOWN_SYMBOL
} Tokenizer_Retval;

typedef struct {
        int line_num;
        Tokenizer_Retval retval;
        char data[64];
} Debug_Info;

Debug_Info tokenize_buffer(Big_Str *source, ARRAY_NAME(Token) *token_array);

#endif
