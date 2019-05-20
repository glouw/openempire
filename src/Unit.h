#pragma once

#include "Point.h"
#include "Graphics.h"

typedef struct
{
    Point cart_point;
    Point cart_fractional;
    bool selected;
    Graphics file;
}
Unit;
