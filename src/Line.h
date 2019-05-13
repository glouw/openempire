#pragma once

#include "Point.h"

typedef struct
{
    Point inner;
    Point outer;
    int32_t tile_file;
}
Line;
