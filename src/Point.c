#include "Point.h"

#include "Util.h"

#include <stdio.h>
#include <stdlib.h>

bool Point_Equal(const Point a, const Point b)
{
    return a.x == b.x
        && a.y == b.y;
}

void Point_Print(const Point a)
{
    printf("%ld %ld\n", a.x, a.y);
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

Point Point_Div(const Point a, const int64_t n)
{
    const Point out = {
        a.x / n,
        a.y / n,
    };
    return out;
}

Point Point_Mul(const Point a, const int64_t n)
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

int64_t Point_Mag(const Point point)
{
    return Util_Sqrt(point.x * point.x + point.y * point.y);
}

Point Point_Normalize(const Point point, const int64_t normal)
{
    const int64_t magnitude = Point_Mag(point);
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

int32_t Point_Slope(const Point point)
{
    return point.y / (point.x == 0 ? 1 : point.x); // Close enough approximation.
}
