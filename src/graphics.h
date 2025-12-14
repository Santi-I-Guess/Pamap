#ifndef GRAPHICS_H
#define GRAPHICS_H 1

#include <raylib.h>

#include "structure.h"

typedef struct {
        bool is_mouse_disabled;
        float move_speed;
        Vector3 movement;
        Vector3 rotation;
} Settings;

void draw_edge(Coordinate first, Coordinate second, Color color);
void draw_trail(Trail trail);

// handle custom camera controls, update camera at the end
void handle_controls(Camera3D *camera, Settings *settings);

// draw controls box and postion / angle box
void draw_info_boxes(Camera3D *camera, Structure *structure);

#endif
