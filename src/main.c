#include <stdio.h>

#include <raylib.h>

#include "graphics.h"
#include "parser/parser.h"
#include "structure.h"

#ifdef DEBUG
#include "debug_funcs.h"
#endif

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
        InitWindow(width, height, "Pathways Mapper");
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

                                for (size_t i = 0; i < structure.edge_count; i++) {
                                        Coordinate first = structure.edges[i].first;
                                        Coordinate second = structure.edges[i].second;
                                        // intertrail color: BLACK
                                        draw_edge(first, second, BLACK);
                                }
                                for (size_t i = 0; i < structure.trails.count; i++) {
                                        draw_trail(structure.trails.data[i]);
                                }

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
