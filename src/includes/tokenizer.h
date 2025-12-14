#ifndef TOKENIZER_H
#define TOKENIZER_H 1

#include <stdio.h>

#include "generic_array.h"
#include "types.h"

typedef struct {
        char data[64];
        int line_num;
        Retval retval;
} Debug_Info;


void store_input(FILE *source_file, Big_Str *source) ;

// tokenize buffer. return debug info, if error occurs
Debug_Info tokenize_buffer(Big_Str *source, ARRAY_NAME(Token) *token_array);

#endif
