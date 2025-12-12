#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug_funcs.h"
#include "parser/parser.h"
#include "parser/tokenizer.h"
#include "structure.h"
#include "types.h"

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

int main(int argc, char **argv) {
        if (argc != 2) {
                printf("usage: pathways <input_file>\n");
                return 0;
        }

        // store file in string
        FILE *source_file = fopen(argv[1], "r");
        if (!source_file) {
                fprintf(stderr, "failed to open file\n");
                return 1;
        }
        Big_Str source = { 0 };
        store_input(source_file, &source);
        fclose(source_file);

        ARRAY_NAME(Token) token_array = {0};
        if (!INIT_FUNC(Token, &token_array, 128)) {
                fprintf(stderr, "failed to init tarray\n");
                free(source.data);
                return 1;
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
                return 0;
        case UNKNOWN_SYMBOL:
                printf("Error: unknown symbol \"%c\" on line %d\n", 
                       context.data[0], context.line_num);
                free(source.data);
                FREE_FUNC(Token, &token_array);
                return 0;
        }
        free(source.data);

        // store into structure
        Structure structure = {0};
        init_structure(&structure);
        Retval populate_retval = populate_structure(&structure, &token_array);
        switch (populate_retval) {
        case GOOD:
                break;
        case UNKNOWN_SYMBOL: // not used in populate_structure
                break;
        case COORDINATE_MALFORM:
                printf("Error: coordinate malform in populate_structure\n");
                FREE_FUNC(Trail, &(structure.trails));
                return 0;
        case TRAIL_BRACE_MALFORM:
                printf("Error: trail brace malform in populate_structure\n");
                FREE_FUNC(Trail, &(structure.trails));
                return 0;
        case MEMORY_ERROR:
                printf("Error: memory error in populate_structure\n");
                FREE_FUNC(Trail, &(structure.trails));
                return 0;
        }

        // run main program
        // print_tokens(token_array);
        print_structure(&structure);

        FREE_FUNC(Token, &token_array);
        free_structure(&structure);
        source.length = 0;
        return 0;
}
