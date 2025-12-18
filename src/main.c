#include <stdarg.h>
#include <stdio.h>

#include <raylib.h>

#include "includes/debug_funcs.h"
#include "includes/graphics.h"
#include "includes/parser.h"
#include "includes/structure.h"

#define UNUSED(x) (void)x

void generate_structure(Structure *structure, Settings *settings);

void custom_log(int msg_type, const char *text, va_list args) {
        UNUSED(msg_type);
        UNUSED(text);
        UNUSED(args);
        return;
}

int main(int argc, char **argv) {
        if (argc < 2) {
                printf("usage: pathways <input_file> [-t]\n");
                return 0;
        }

        // tokenize, parse, and populate data into structure
        Structure structure = {0};
        Settings settings = {0};
        settings.is_mouse_disabled = true;
        settings.move_speed = 0.5f;

        char filename[128] = "";
        for (int i = 1; i < argc; ++i) {
                if (strcmp(argv[i], "-t") == 0)
                        settings.is_testing = true;
                else if (strlen(filename) == 0)
                        strcpy(filename, argv[i]);
        }
        if (strlen(filename) == 0) {
                printf("failed to provide a filename\n");
                return 0;
        }
        settings.source_file = fopen(filename, "r");
        if (settings.source_file == NULL) {
                fprintf(stderr, "failed to open file\n");
                return 1;
        }

        generate_structure(&structure, &settings);

        if (settings.is_testing) {
                print_structure(&structure);
                print_settings(&settings);
                free_structure(&structure);
                return 0;
        }

        SetTraceLogCallback(custom_log);

        const int width = 1000;
        const int height = 600;
        InitWindow(width, height, "Pathways Mapper");
        SetTargetFPS(60);
        Camera3D camera   = { 0 };
        camera.position.x = (float)settings.start_x,
        camera.position.y = (float)settings.start_y,
        camera.position.z = (float)settings.start_z,
        camera.target.x   = (float)settings.start_view_x,
        camera.target.y   = (float)settings.start_view_y,
        camera.target.z   = (float)settings.start_view_z,
        camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
        camera.fovy       = 45.0f;
        camera.projection = CAMERA_PERSPECTIVE;

        DisableCursor();
        SetExitKey(KEY_NULL);

        while (!WindowShouldClose()) {
                UpdateCamera(&camera, CAMERA_CUSTOM);

                if (IsKeyPressed(KEY_Q)) {
                        if (settings.is_mouse_disabled)
                                DisableCursor();
                        else
                                EnableCursor();
                        settings.is_mouse_disabled ^= true;
                } else if (IsKeyPressed(KEY_R)) {
                        free_structure(&structure);
                        generate_structure(&structure, &settings);
                        camera.position.x = (float)settings.start_x;
                        camera.position.y = (float)settings.start_y;
                        camera.position.z = (float)settings.start_z;
                        camera.target.x = (float)settings.start_view_x;
                        camera.target.y = (float)settings.start_view_y;
                        camera.target.z = (float)settings.start_view_z;
                } else if (IsKeyPressed(KEY_V)) {
                        camera.position.x = (float)settings.tele_x;
                        camera.position.y = (float)settings.tele_y;
                        camera.position.z = (float)settings.tele_z;
                        camera.target.x = (float)settings.start_view_x;
                        camera.target.y = (float)settings.start_view_y;
                        camera.target.z = (float)settings.start_view_z;
                } else if (IsKeyPressed(KEY_Z)) {
                        // i need to set target in the direction of the
                        //      origin without changing the radius
                        camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
                }
                handle_controls(&camera, &settings);
#ifdef DEBUG
                Vector2 mouse_d = GetMouseDelta();
                printf("%.1f, %.1f\n", mouse_d.x, mouse_d.y);
#endif
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
                draw_info_boxes(&camera, &settings);
                EndDrawing();
        }

        CloseWindow();

        free_structure(&structure);
        return 0;
}

void generate_structure(Structure *structure, Settings *settings) {
        settings->start_x = 10;
        settings->start_y = 10;
        settings->start_z = 10;
        settings->start_view_x = 0;
        settings->start_view_y = 0;
        settings->start_view_z = 0;
        settings->tele_x = 10;
        settings->tele_y = 10;
        settings->tele_z = 10;

        // store file in string
        Big_Str source = { 0 };
        store_input(settings->source_file, &source);
        fclose(settings->source_file);
        settings->source_file = NULL;

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
        Retval populate_retval = populate_structure(
                structure,
                settings,
                &token_array
        );
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

        FREE_FUNC(Token, &token_array);
}
