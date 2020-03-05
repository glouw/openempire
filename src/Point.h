#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    int32_t x;
    int32_t y;
}
Point;

bool Point_Equal(const Point a, const Point b);

void Point_Print(const Point a);

Point Point_Add(const Point a, const Point b);

Point Point_Sub(const Point a, const Point b);

Point Point_Div(const Point a, const int32_t n);

Point Point_Mul(const Point a, const int32_t n);

Point Point_ToIso(const Point);

Point Point_ToCart(const Point);

bool Point_IsZero(const Point);

int32_t Point_Mag(const Point);

Point Point_Normalize(const Point, const int32_t normal);

Point Point_Dot(const Point a, const Point b);

int32_t Point_Slope(const Point);

Point Point_Rand(void);

Point Point_Wrap(const int32_t index, const int32_t width, const int32_t res);

Point Point_Layout(const int32_t index, const int32_t xres, const int32_t yres);

bool Point_IsEven(const Point);

uint64_t Point_Flatten(const Point);

int32_t Point_GetYFromLine(const Point i, const Point j, const int32_t x);

int32_t Point_GetXFromLine(const Point i, const Point j, const int32_t y);
