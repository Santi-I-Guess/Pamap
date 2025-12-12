#ifndef TOKENIZER_H
#define TOKENIZER_H 1

#include "../generic_array.h"
#include "../types.h"

typedef struct {
        char data[64];
        int line_num;
        Retval retval;
} Debug_Info;

// tokenize buffer. return debug info, if error occurs
Debug_Info tokenize_buffer(Big_Str *source, ARRAY_NAME(Token) *token_array);

#endif
