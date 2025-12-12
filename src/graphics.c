#include <raylib.h>

#include "structure.h"

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
// void DrawCylinder(Vector3 position, float radiusTop, float radiusBottom, float height, int slices, Color color)

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
