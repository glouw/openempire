#pragma once

#include "Points.h"

typedef struct
{
    Point a;
    Point b;
    Point c;
    Point d;
}
Quad;

Points Quad_GetRenderPoints(const Quad);
