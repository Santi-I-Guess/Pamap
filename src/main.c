#include <stdio.h>

#include <raylib.h>

#include "graphics.h"
#include "parser/parser.h"
#include "structure.h"

#ifdef DEBUG
#include "debug_funcs.h"
#endif

void generate_structure(Structure *structure, char **argv);

int main(int argc, char **argv) {
        if (argc != 2) {
                printf("usage: pathways <input_file>\n");
                return 0;
        }

        // tokenize, parse, and populate data into structure
        Structure structure = {0};
        generate_structure(&structure, argv);

        const int width = 1000;
        const int height = 600;
        InitWindow(width, height, "Pathways Mapper");
        SetTargetFPS(60);
        Camera3D camera   = { 0 };
        camera.position   = (Vector3){
                (float)structure.start_x,
                (float)structure.start_y,
                (float)structure.start_z,
        };
        camera.target     = (Vector3){ 0.0f, 0.0f, 0.0f };
        camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
        camera.fovy       = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;

        Settings settings = {
                .is_mouse_disabled = true,
                .move_speed = 1.0f,
                .movement = {0},
                .rotation = {0},
        };

        DisableCursor();
        SetExitKey(KEY_NULL);

        while (!WindowShouldClose()) {
                UpdateCamera(&camera, CAMERA_CUSTOM);

                handle_controls(&camera, &settings);
                if (IsKeyPressed(KEY_Q)) {
                        if (settings.is_mouse_disabled)
                                DisableCursor();
                        else
                                EnableCursor();
                        settings.is_mouse_disabled ^= true;
                } else if (IsKeyPressed(KEY_R)) {
                        free_structure(&structure);
                        generate_structure(&structure, argv);
                        camera.position   = (Vector3){
                                (float)structure.start_x,
                                (float)structure.start_y,
                                (float)structure.start_z,
                        };
                } else if (IsKeyPressed(KEY_V)) {
                        camera.position   = (Vector3){
                                (float)structure.tele_x,
                                (float)structure.tele_y,
                                (float)structure.tele_z,
                        };
                } else if (IsKeyPressed(KEY_Z)) {
                        // i need to set target in the direction of the
                        //      origin without changing the radius
                        camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
                }

                BeginDrawing();
                ClearBackground(RAYWHITE);
                BeginMode3D(camera);

                for (size_t i = 0; i < structure.edge_count; i++) {
                        Coordinate first = structure.edges[i].first;
                        Coordinate second = structure.edges[i].second;
                        // intertrail color: BLACK
                        draw_edge(first, second, BLACK);
                }
                for (size_t i = 0; i < structure.trails.count; i++) {
                        draw_trail(structure.trails.data[i]);
                }
                DrawGrid(10, 1.0f);

#ifdef DEBUG
                Vector3 tar_pos = {
                        .x = camera.target.x,
                        .y = camera.target.y,
                        .z = camera.target.z
                };
                DrawSphere(tar_pos, 0.25, RED);
#endif

                EndMode3D();
                // controls box and position / angle box
                draw_info_boxes(&camera, &structure);
                EndDrawing();
        }

        CloseWindow();

        free_structure(&structure);
        return 0;
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
        case COORDINATE_MALFORM:  // not used in tokenize_buffer
        case EXPECTED_MACRO_ARG:  // not used in tokenize_buffer
        case MACRO_ARG_MALFORM:   // not used in tokenize_buffer
        case TRAIL_BRACE_MALFORM: // not used in tokenize_buffer
        case UNKNOWN_MACRO: // not used in tokenize_buffer
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
        case COORDINATE_MALFORM:
                printf("Error: coordinate malform in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case EXPECTED_MACRO_ARG:
                printf("Error: expected macro arg in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case MACRO_ARG_MALFORM:
                printf("Error: macro arg malform in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case MEMORY_ERROR:
                printf("Error: memory error in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case TRAIL_BRACE_MALFORM:
                printf("Error: trail brace malform in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case UNKNOWN_MACRO:
                printf("Error: unknown macro in populate_structure\n");
                FREE_FUNC(Trail, &(structure->trails));
                exit(0);
        case UNKNOWN_SYMBOL: // not used in populate_structure
                break;
        }

#ifdef DEBUG
        print_tokens(token_array);
#endif

        FREE_FUNC(Token, &token_array);
}
