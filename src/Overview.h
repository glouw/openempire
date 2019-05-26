#pragma once

#include "Input.h"
#include "Point.h"
#include "Rect.h"
#include "Grid.h"
#include "Quad.h"

typedef struct
{
    Point point;
    Grid grid;
    int32_t xres;
    int32_t yres;
    Rect selection_box;
}
Overview;

Overview Overview_Init(const int32_t xres, const int32_t yres, const Grid);

Overview Overview_Update(Overview, const Input);

Point Overview_IsoToCart(const Overview, const Point, const bool raw);

Point Overview_CartToIso(const Overview, const Point);

Quad Overview_GetRenderBox(const Overview, const int32_t border);

Point Overview_IsoSnapTo(const Overview, const Point);

bool Overview_IsSelectionBoxBigEnough(const Overview);
