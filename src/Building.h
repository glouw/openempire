#pragma once

#include "Color.h"
#include "Trait.h"
#include "Point.h"
#include "Color.h"
#include "Graphics.h"

typedef struct
{
    Trait trait;
    Point cart;
    Color color;
    Graphics file;
    int32_t health;
}
Building;

Building Building_Make(const Point, const Graphics, const Color);
