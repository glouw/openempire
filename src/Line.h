#pragma once

#include "Point.h"
#include "Terrain.h"

typedef struct
{
    Point inner;
    Point outer;
    Terrain tile_file;
}
Line;
