#ifndef GRAPHICS_H
#define GRAPHICS_H 1

#include <stdbool.h>
#include <stdio.h>
#include <raylib.h>

#include "structure.h"

typedef struct {
        bool is_mouse_disabled;
        bool is_testing;
        float move_speed;
        FILE *source_file;
        int start_x;
        int start_y;
        int start_z;
        int start_view_x;
        int start_view_y;
        int start_view_z;
        int tele_x;
        int tele_y;
        int tele_z;
} Settings;

void draw_edge(Coordinate first, Coordinate second, Color color);
void draw_trail(Trail trail);

// handle custom camera controls, update camera at the end
void handle_controls(Camera3D *camera, Settings *settings);

// draw controls box and postion / angle box
void draw_info_boxes(
        Camera3D *camera,
        Settings *settings
);

#endif
