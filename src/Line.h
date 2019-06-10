#pragma once

#include "Point.h"
#include "Terrain.h"

typedef struct
{
    Point inner;
    Point outer;
    Terrain file;
}
Line;
