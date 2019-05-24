#pragma once

#include "Points.h"
#include "Graphics.h"
#include "Grid.h"

typedef struct
{
    Point cart_point;
    Point cart_fractional_local;
    Points path;
    int32_t path_index;
    bool selected;
    Graphics file;
}
Unit;

Unit Unit_Move(Unit, const Grid);
