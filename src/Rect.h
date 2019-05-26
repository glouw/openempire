#pragma once

#include "Point.h"
#include "Grid.h"

typedef struct
{
    Point a;
    Point b;
}
Rect;

Rect Rect_GetGridCenter(const Grid);

Rect Rect_GetEllipse(const Point);

int32_t Rect_GetArea(const Rect);

Rect Rect_CorrectOrientation(const Rect);
