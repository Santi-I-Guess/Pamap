#ifndef GRAPHICS_H
#define GRAPHICS_H 1

#include <raylib.h>

#include "structure.h"

void draw_edge(Coordinate first, Coordinate second, Color color);
void draw_trail(Trail trail);

#endif
