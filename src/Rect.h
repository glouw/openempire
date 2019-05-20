#pragma once

#include "Tile.h"

// a---------+
// |         |
// |         |
// |         |
// +---------b

typedef struct
{
    Point a;
    Point b;
}
Rect;

Rect Rect_Get(const Tile);
