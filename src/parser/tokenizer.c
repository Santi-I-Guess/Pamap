#include <ctype.h>

#include "../generic_array.h"
#include "../types.h"
#include "tokenizer.h"

// return debug info, if error occurs
Debug_Info tokenize_buffer(Big_Str *source, ARRAY_NAME(Token) *token_array) {
        // debug info
        Debug_Info context = {.line_num = -1, .retval = GOOD, .data = ""};
        int line_num = 1;

        size_t buff_idx = 0;
        while (buff_idx < source->length) {
                // skip whitespace
                while (buff_idx < source->length) {
                        char curr = source->data[buff_idx];
                        if (!isspace(curr))
                                break;
                        if (curr == '\n')
                                line_num++;
                        buff_idx++;
                }
                if (buff_idx == source->length)
                        return context; // normal exit

                // trail borders don't consume inner contents
                char curr = source->data[buff_idx];
                Token temp_token;
                temp_token.line_num = line_num;
                bool tarray_res = true;
                switch (curr) {
                case '[': // trail start
                        temp_token.data[0] = '[';
                        tarray_res = APPEND_FUNC(Token,token_array,temp_token);
                        buff_idx++;
                        continue; // next token
                case ']':
                        temp_token.data[0] = ']';
                        tarray_res = APPEND_FUNC(Token,token_array,temp_token);
                        buff_idx++;
                        continue; // next token
                case ',':
                        buff_idx++;
                        continue; // next token
                default:
                        break;
                }
                // if append failed
                if (!tarray_res) {
                        context.line_num = line_num;
                        context.retval = MEMORY_ERROR;
                        return context;
                }

                // check if buff_idx is pointing to a token indicator
                char ending_char = '\0';
                switch (curr) {
                case '(': // coord start
                        ending_char = ')';
                        break;
                case '{': // edge start
                        ending_char = '}';
                        break;
                default:
                        context.line_num = line_num;
                        context.retval = UNKNOWN_SYMBOL;
                        context.data[0] = curr;
                        return context;
                }

                // keep grabbing tokens until appropriate delimiter is hit
                size_t token_len = 0;
                Token curr_token = { .data = "", .line_num = line_num };
                while (buff_idx + token_len < source->length) {
                        char curr = source->data[buff_idx + token_len];
                        curr_token.data[token_len] = curr;
                        if (curr == ending_char)
                                break;
                        token_len++;
                }
                tarray_res = APPEND_FUNC(Token,token_array,curr_token);
                if (!tarray_res) {
                        context.line_num = line_num;
                        context.retval = MEMORY_ERROR;
                        return context;
                }
                buff_idx += token_len + 1;
        }

        return context; // normal exit
}
