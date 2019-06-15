#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    int64_t x;
    int64_t y;
}
Point;

bool Point_Equal(const Point a, const Point b);

void Point_Print(const Point a);

Point Point_Add(const Point a, const Point b);

Point Point_Sub(const Point a, const Point b);

Point Point_Div(const Point a, const int64_t n);

Point Point_Mul(const Point a, const int64_t n);

Point Point_ToIso(const Point);

Point Point_ToCart(const Point);

bool Point_IsZero(const Point);

int64_t Point_Mag(const Point);

Point Point_Normalize(const Point, const int64_t normal);

Point Point_Dot(const Point a, const Point b);

int32_t Point_Slope(const Point);
