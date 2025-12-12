#include <ctype.h>
#include <stdio.h>

#include "../generic_array.h"
#include "../types.h"
#include "tokenizer.h"

void store_input(FILE *source_file, Big_Str *source) {
        fseek(source_file, 0, SEEK_END);
        long file_size = ftell(source_file);
        fseek(source_file, 0, SEEK_SET);

        source->data = (char*)calloc((size_t)(file_size + 4), sizeof(char));
        if (!source->data) {
                fprintf(stderr, "source->data failed to init");
                exit(1);
        }

        while (!feof(source_file)) {
                char curr = (char)fgetc(source_file);
                if (isprint(curr) || isspace(curr)) {
                        source->data[source->length] = curr;
                        (source->length)++;
                }
        }
}

Debug_Info tokenize_buffer(Big_Str *source, ARRAY_NAME(Token) *token_array) {
        // debug info
        Debug_Info context = {.line_num = -1, .retval = GOOD, .data = ""};
        int line_num = 1;

        size_t buff_idx = 0;
next_token:
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

                // ignore comments (//) to newline
                if (curr == '/') {
                        if (buff_idx == source->length - 1) {
                                context.data[0] = curr;
                                context.line_num = line_num;
                                context.retval = UNKNOWN_SYMBOL;
                                return context;
                        }
                        if (source->data[buff_idx + 1] != '/') {
                                context.data[0] = source->data[buff_idx + 1];
                                context.line_num = line_num;
                                context.retval = UNKNOWN_SYMBOL;
                                return context;
                        }
                        while (buff_idx < source->length) {
                                curr = source->data[buff_idx];
                                buff_idx++;
                                if (curr == '\n') {
                                        line_num++;
                                        goto next_token;
                                }
                        }
                }

                Token temp_token = { .data = "", .line_num = line_num };
                bool tarray_res = true;
                switch (curr) {
                case '[': // trail start
                        temp_token.data[0] = '[';
                        tarray_res = APPEND_FUNC(Token, token_array, temp_token);
                        if (!tarray_res) {
                        }
                        buff_idx++;
                        continue; // next token
                case ']':
                        temp_token.data[0] = ']';
                        tarray_res = APPEND_FUNC(Token, token_array, temp_token);
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
                        context.data[0] = curr;
                        context.line_num = line_num;
                        context.retval = UNKNOWN_SYMBOL;
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
                tarray_res = APPEND_FUNC(Token, token_array, curr_token);
                if (!tarray_res) {
                        context.line_num = line_num;
                        context.retval = MEMORY_ERROR;
                        return context;
                }
                buff_idx += token_len + 1;
        }

        return context; // normal exit
}
