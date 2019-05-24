#pragma once

#include "Points.h"
#include "Graphics.h"
#include "Grid.h"

typedef struct
{
    Point cart;
    Point cart_fractional_local;
    Point cart_fractional_local_goal;
    Points path;
    int32_t path_index;
    bool selected;
    Graphics file;
}
Unit;

Unit Unit_Move(Unit, const Grid);
