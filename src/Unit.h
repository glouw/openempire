#pragma once

#include "Points.h"
#include "Graphics.h"
#include "Grid.h"

typedef struct
{
    Point cart;
    Point cart_grid_offset;
    Point cart_grid_offset_goal;
    Point cell;
    int32_t max_speed;
    int32_t accel;
    Point velocity;
    Points path;
    int32_t path_index;
    bool selected;
    Graphics file;
    const char* file_name;
}
Unit;

Unit Unit_Move(Unit, const Grid);

Unit Unit_Make(const Point, const Grid, const Graphics);
