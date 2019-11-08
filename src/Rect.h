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

Rect Rect_GetEllipse(const Point, const int32_t width);

int32_t Rect_GetArea(const Rect);

Rect Rect_CorrectOrientation(const Rect);

bool Rect_ContainsPoint(const Rect, const Point);

bool Rect_SeesRect(const Rect out, const Rect in);

bool Rect_OnScreen(const Rect outline, const Rect rect);

bool Rect_TotallyOffScreen(const Rect outline, const Rect rect);
