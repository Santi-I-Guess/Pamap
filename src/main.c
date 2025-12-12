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
        // Camera looking at point
        camera.target     = (Vector3){ 0.0f, 0.0f, 0.0f };
        // Camera up vector (rotation towards target)
        camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
        // Camera field-of-view Y
        camera.fovy       = 45.0f;
        // Camera projection type
        camera.projection = CAMERA_PERSPECTIVE;

        DisableCursor();
        bool mouse_toggled_off = true;
        SetExitKey(KEY_NULL);
        while (!WindowShouldClose()) {
                UpdateCamera(&camera, CAMERA_FREE);

                if (IsKeyPressed(KEY_Q)) {
                        if (mouse_toggled_off)
                                DisableCursor();
                        else
                                EnableCursor();
                        mouse_toggled_off ^= true;
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
                        camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
                }

                char teleport_buffer[64];
                sprintf(teleport_buffer, "- V to teleport to (%d, %d, %d)",
                        structure.tele_x, structure.tele_y, structure.tele_z);

                char position_buffer[64];
                sprintf(position_buffer, "Current pos: (%.0f, %.0f, %.0f)",
                        camera.position.x, camera.position.y, camera.position.z);

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
                                EndMode3D();

                        // draw controls
                        DrawRectangle( 10, 10, 300, 150, Fade(SKYBLUE, 0.5f));
                        DrawRectangleLines( 10, 10, 300, 150, BLUE);
                        DrawText("Free camera default controls:", 20,  20, 10, BLACK);
                        DrawText("- Mouse Wheel to Zoom in-out",  40,  40, 10, DARKGRAY);
                        DrawText("- Mouse Wheel Pressed to Pan",  40,  60, 10, DARKGRAY);
                        DrawText("- Q to toggle the cursor",      40,  80, 10, DARKGRAY);
                        DrawText("- R to reload the file"  ,      40, 100, 10, DARKGRAY);
                        DrawText(teleport_buffer,                 40, 120, 10, DARKGRAY);
                        DrawText("- Z to zoom to (0, 0, 0)",      40, 140, 10, DARKGRAY);

                        // draw camera position
                        DrawRectangle( 820, 10, 160, 30, Fade(RED, 0.5f));
                        DrawRectangleLines( 820, 10, 160, 30, BLUE);
                        DrawText(position_buffer, 830, 20, 10, DARKGRAY);
                EndDrawing();
        }

        CloseWindow();

        free_structure(&structure);
        return 0;
}
