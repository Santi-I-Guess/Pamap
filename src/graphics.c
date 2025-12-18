#include <math.h>

#include <raylib.h>

#include "includes/structure.h"
#include "includes/graphics.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#define CLAMP(x, a, b) ((x > b) ? b : (x < a ? a : x));

void draw_edge(Coordinate first, Coordinate second, Color color) {
        Vector3 first_vec = {
                .x = (float)first.x,
                .y = (float)first.y,
                .z = (float)first.z
        };
        Vector3 second_vec = {
                .x = (float)second.x,
                .y = (float)second.y,
                .z = (float)second.z
        };
        DrawCylinderEx(first_vec, second_vec, 0.1f, 0.1f, 10, color);
}


// void DrawSphere(Vector3 centerPos, float radius, Color color);
// void DrawCylinder(Vector3 position, float radiusTop, float radiusBottom,
//      float height, int slices, Color color)

void draw_trail(Trail trail) {
        Coordinate prev_coord = {0};
        for (size_t i = 0; i < trail.count; i++) {
                Coordinate curr_coord = trail.data[i];
                Vector3 curr_vec = {
                        .x = (float)curr_coord.x,
                        .y = (float)curr_coord.y,
                        .z = (float)curr_coord.z
                };
                Color node_color = {
                        trail.node_red,
                        trail.node_green,
                        trail.node_blue,
                        255
                };
                Color edge_color = {
                        trail.edge_red,
                        trail.edge_green,
                        trail.edge_blue,
                        255
                };
                DrawSphere(curr_vec, 0.5, node_color);
                if (i > 0)
                        draw_edge(curr_coord, prev_coord, edge_color);
                prev_coord = curr_coord;
        }
}


void handle_controls(Camera3D *camera, Settings *settings) {
        // W, S: move "forward" and "backward"
        // A, D: move "left" and "right"
        // space: elevate
        // shift: descent
        // mouse wheel: up to double translation speed, down to half it

        // auxiliary vars
        float diff_x = camera->target.x - camera->position.x;
        float diff_y = camera->target.y - camera->position.y;
        float diff_z = camera->target.z - camera->position.z;
        float radius = sqrtf(
                (diff_x * diff_x) + (diff_y * diff_y) + (diff_z * diff_z)
        );
        float radius_xz = sqrtf((diff_x * diff_x) + (diff_z * diff_z));
        float theta = atan2f(diff_z, diff_x);
        float phi   = acosf(diff_y / radius);

        // handle elevation controls
        if (IsKeyDown(KEY_SPACE)) {
                camera->position.y += settings->move_speed;
                camera->target.y   += settings->move_speed;
        } else if (IsKeyDown(KEY_LEFT_SHIFT)) {
                camera->position.y -= settings->move_speed;
                camera->target.y   -= settings->move_speed;
        }

        // handle camera rotation
        // even w/o movement, mouse deltas spike at beginning of program
        Vector2 mouse_deltas = GetMouseDelta();
        float new_theta      = theta + (0.01f * mouse_deltas.x);
        float new_phi        = phi   + (0.01f * mouse_deltas.y);
        new_phi = CLAMP(new_phi, 0.1f, 3.13f);
        if (fabs(mouse_deltas.x) > 0.1f) {
                float change_x   = radius_xz * cosf(new_theta);
                float change_z   = radius_xz * sinf(new_theta);
                camera->target.x = change_x + camera->position.x;
                camera->target.z = change_z + camera->position.z;
        } if (fabs(mouse_deltas.y) > 0.1f) {
                float change_x   = radius * sinf(new_phi) * cosf(new_theta);
                float change_z   = radius * sinf(new_phi) * sinf(new_theta);
                float change_y   = radius * cosf(new_phi);
                camera->target.x = change_x + camera->position.x;
                camera->target.y = change_y + camera->position.y;
                camera->target.z = change_z + camera->position.z;
        }

        // handle lateral translation
        float base_change_x = 0;
        float base_change_z = 0;
        float chosen_angle = theta;
        if (IsKeyDown(KEY_A)) {
                // left
                chosen_angle   = theta + PI/2.0f;
                base_change_x  = -1;
                base_change_z  = -1;
                base_change_x *= settings->move_speed * cosf(chosen_angle);
                base_change_z *= settings->move_speed * sinf(chosen_angle);
                camera->position.x += base_change_x;
                camera->position.z += base_change_z;
                camera->target.x   += base_change_x;
                camera->target.z   += base_change_z;
        } else if (IsKeyDown(KEY_D)) {
                // right
                chosen_angle   = theta + PI/2.0f;
                base_change_x  = 1;
                base_change_z  = 1;
                base_change_x *= settings->move_speed * cosf(chosen_angle);
                base_change_z *= settings->move_speed * sinf(chosen_angle);
                camera->position.x += base_change_x;
                camera->position.z += base_change_z;
                camera->target.x   += base_change_x;
                camera->target.z   += base_change_z;
        } else if (IsKeyDown(KEY_W)) {
                // forwards
                base_change_x  = 1;
                base_change_z  = 1;
                base_change_x *= settings->move_speed * cosf(chosen_angle);
                base_change_z *= settings->move_speed * sinf(chosen_angle);
                camera->position.x += base_change_x;
                camera->position.z += base_change_z;
                camera->target.x   += base_change_x;
                camera->target.z   += base_change_z;
        } else if (IsKeyDown(KEY_S)) {
                // backwards
                base_change_x  = -1;
                base_change_z  = -1;
                base_change_x *= settings->move_speed * cosf(chosen_angle);
                base_change_z *= settings->move_speed * sinf(chosen_angle);
                camera->position.x += base_change_x;
                camera->position.z += base_change_z;
                camera->target.x   += base_change_x;
                camera->target.z   += base_change_z;
        }

        // handle speed
        float mouse_wheel = GetMouseWheelMove();
        if (mouse_wheel > 0.1f) {
                settings->move_speed *= 1.1f;
        } else if (mouse_wheel < -0.1f) {
                settings->move_speed *= 0.9f;
        }
}

void draw_info_boxes(
        Camera3D *camera,
        Settings *settings
) {
        char teleport_buffer[64];
        char position_buffer[64];
        char target_buffer[64];
        sprintf(teleport_buffer, "- V to teleport to (%d, %d, %d)",
                settings->tele_x,
                settings->tele_y,
                settings->tele_z
        );
        sprintf(position_buffer, "Current pos: (%.0f, %.0f, %.0f)",
                camera->position.x,
                camera->position.y,
                camera->position.z
        );
        sprintf(target_buffer, "Current tar: (%.0f, %.0f, %.0f)",
                camera->target.x,
                camera->target.y,
                camera->target.z
        );

        float diff_x = camera->target.x - camera->position.x;
        float diff_y = camera->target.y - camera->position.y;
        float diff_z = camera->target.z - camera->position.z;
        float radius = sqrtf(
                (diff_x * diff_x)
                + (diff_y * diff_y)
                + (diff_z * diff_z)
        );
        float radius_xz = sqrtf((diff_x * diff_x) + (diff_z * diff_z));
        float theta     = atan2f(diff_z, diff_x);
        float phi       = acosf(diff_y / radius);
        char theta_buffer[64];
        char phi_buffer[64];
        char radius_buffer[64];
        char xz_radius_buffer[64];
        char move_speed_buffer[64];

        // write position info to buffers
        sprintf(theta_buffer,      "Current theta: %.1f r", theta);
        sprintf(phi_buffer,        "Current phi: %.1f r",   phi);
        sprintf(radius_buffer,     "Current radius: %.1f",  radius);
        sprintf(xz_radius_buffer,  "Current xz-rad: %.1f",  radius_xz);
        sprintf(move_speed_buffer, "Current speed: %.2f",   settings->move_speed);

        // draw controls info
        DrawRectangle( 10, 10, 300, 150, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines( 10, 10, 300, 150, BLUE);
        DrawText("Free camera default controls:", 20,  20, 10, BLACK);
        DrawText("- Mouse Wheel to Zoom in-out",  40,  40, 10, DARKGRAY);
        DrawText("- Mouse Wheel Pressed to Pan",  40,  60, 10, DARKGRAY);
        DrawText("- Q to toggle the cursor",      40,  80, 10, DARKGRAY);
        DrawText("- R to reload the file"  ,      40, 100, 10, DARKGRAY);
        DrawText(teleport_buffer,                 40, 120, 10, DARKGRAY);
        DrawText("- Z to zoom to (0, 0, 0)",      40, 140, 10, DARKGRAY);

        // draw camera position info
#ifndef DEBUG
        int num_vars = 2;
        DrawRectangle(              820, 10, 160, 10+(20*num_vars), Fade(RED, 0.5f));
        DrawRectangleLines(         820, 10, 160, 10+(20*num_vars), BLUE);
        DrawText(position_buffer,   830, 20,  10, DARKGRAY);
        DrawText(move_speed_buffer, 830, 40,  10, DARKGRAY);
#else
        int num_vars = 7;
        DrawRectangle(              820, 10, 160, 10+(20*num_vars), Fade(RED, 0.5f));
        DrawRectangleLines(         820, 10, 160, 10+(20*num_vars), BLUE);
        DrawText(position_buffer,   830, 20,  10, DARKGRAY);
        DrawText(target_buffer,     830, 40,  10, DARKGRAY);
        DrawText(theta_buffer,      830, 60,  10, DARKGRAY);
        DrawText(phi_buffer,        830, 80,  10, DARKGRAY);
        DrawText(radius_buffer,     830, 100, 10, DARKGRAY);
        DrawText(xz_radius_buffer,  830, 120, 10, DARKGRAY);
        DrawText(move_speed_buffer, 830, 140, 10, DARKGRAY);

#endif
}
