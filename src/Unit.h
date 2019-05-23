#pragma once

#include "Point.h"
#include "Graphics.h"
#include "Grid.h"

typedef struct
{
    Point cart_point;
    Point cart_fractional;
    Point cart_goal;
    bool selected;
    Graphics file;
}
Unit;

Unit Unit_Move(Unit, const Grid);
