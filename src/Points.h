#pragma once

#include "Point.h"

#include <stdint.h>

typedef struct
{
    Point* point;
    int32_t count;
    int32_t max;
}
Points;

Points Points_New(const int32_t max);

Points Points_Append(Points, const Point);

Points Points_Free(Points);

Points Points_Cat(Points a, const Points b);

void Points_Print(const Points);

Points Points_Reverse(Points);

Points Points_Copy(const Points);
