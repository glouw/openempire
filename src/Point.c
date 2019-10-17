#include "Point.h"

#include "Util.h"
#include "Config.h"

#include <stdio.h>
#include <stdlib.h>

bool Point_Equal(const Point a, const Point b)
{
    return a.x == b.x
        && a.y == b.y;
}

void Point_Print(const Point a)
{
    fprintf(stderr, "%d %d\n", a.x, a.y);
}

Point Point_Add(const Point a, const Point b)
{
    const Point out = {
        a.x + b.x,
        a.y + b.y,
    };
    return out;
}

Point Point_Sub(const Point a, const Point b)
{
    const Point out = {
        a.x - b.x,
        a.y - b.y,
    };
    return out;
}

Point Point_Div(const Point a, const int32_t n)
{
    const Point out = {
        a.x / n,
        a.y / n,
    };
    return out;
}

Point Point_Mul(const Point a, const int32_t n)
{
    const Point out = {
        a.x * n,
        a.y * n,
    };
    return out;
}

Point Point_ToIso(const Point cart)
{
    const Point iso = {
        +(cart.x + cart.y) / 1,
        -(cart.x - cart.y) / 2,
    };
    return iso;
}

Point Point_ToCart(const Point iso)
{
    const Point cart = {
        -(2 * iso.y - iso.x) / 2,
        +(2 * iso.y + iso.x) / 2,
    };
    return cart;
}

bool Point_IsZero(const Point point)
{
    const Point zero = { 0, 0 };
    return Point_Equal(zero, point);
}

int32_t Point_Mag(const Point point)
{
    return Util_Sqrt(
            (uint64_t) point.x * (uint64_t) point.x +
            (uint64_t) point.y * (uint64_t) point.y);
}

Point Point_Normalize(const Point point, const int32_t normal)
{
    const int32_t magnitude = Point_Mag(point);
    const Point elongated = Point_Mul(point, normal);
    return Point_Div(elongated, magnitude);
}

Point Point_Dot(const Point a, const Point b)
{
    const Point out = {
        a.x * b.x,
        a.y * b.y,
    };
    return out;
}

// Rough approximate to 1 as the limit of X goes to zero.
int32_t Point_Slope(const Point point)
{
    return point.y / (point.x == 0 ? 1 : point.x);
}

Point Point_Rand(void)
{
    const Point point = {
        Util_Rand(),
        Util_Rand(),
    };
    return point;
}

Point Point_Wrap(const int32_t index, const int32_t width, const int32_t res)
{
    const int32_t x = (index * width ) % res;
    const int32_t y =   width * ((width * index) / res);
    const Point point = { x, y };
    return point;
}

Point Point_Layout(const int32_t index, const int32_t xres, const int32_t yres)
{
    const int32_t width = CONFIG_ICON_SIZE;
    const int32_t height = CONFIG_ICON_SIZE;
    const int32_t rows = 3;
    const int32_t columns = 5;
    const int32_t res = columns * width;
    const Point center = { xres / 2, yres };
    const Point shift = { width * columns / 2, height * rows };
    const Point start = Point_Sub(center, shift);
    const Point wrap = Point_Wrap(index, width, res);
    return Point_Add(wrap, start);
}
