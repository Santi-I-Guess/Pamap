#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#include "parser/parser.h"
#include "parser/tokenizer.h"
#include "structure.h"
#include "types.h"

#ifdef DEBUG
#include "debug_funcs.h"
#endif

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

int main(int argc, char **argv) {
        if (argc != 2) {
                printf("usage: pathways <input_file>\n");
                return 0;
        }

        // tokenize, parse, and populate data into structure
        Structure structure = {0};
        generate_structure(&structure, argv);

#ifdef DEBUG
        print_structure(&structure);
#endif

        const int width = 800;
        const int height = 450;
        InitWindow(width, height, "pathways mapper");
        SetTargetFPS(60);
        Camera3D camera   = { 0 };
        camera.position   = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
        camera.target     = (Vector3){ 0.0f, 0.0f, 0.0f };    // Camera looking at point
        camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };    // Camera up vector (rotation towards target)
        camera.fovy       = 45.0f;                            // Camera field-of-view Y
        camera.projection = CAMERA_PERSPECTIVE;               // Camera projection type
        Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
        DisableCursor();

        while (!WindowShouldClose())        // Detect window close button or ESC key
        {
                UpdateCamera(&camera, CAMERA_FREE);

                if (IsKeyPressed(KEY_Z))
                        camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };

                BeginDrawing();
                        ClearBackground(RAYWHITE);
                                BeginMode3D(camera);
                                DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
                                DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);
                                DrawGrid(10, 1.0f);
                                EndMode3D();
                        DrawRectangle( 10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
                        DrawRectangleLines( 10, 10, 320, 93, BLUE);
                        DrawText("Free camera default controls:", 20, 20, 10, BLACK);
                        DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, DARKGRAY);
                        DrawText("- Mouse Wheel Pressed to Pan", 40, 60, 10, DARKGRAY);
                        DrawText("- Z to zoom to (0, 0, 0)", 40, 80, 10, DARKGRAY);
                EndDrawing();
        }

        CloseWindow();

        free_structure(&structure);
        return 0;
}
