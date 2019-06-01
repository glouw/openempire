#include "Point.h"

#include "Util.h"

#include <stdio.h>
#include <limits.h>

bool Point_Equal(const Point a, const Point b)
{
    return a.x == b.x
        && a.y == b.y;
}

void Point_Print(const Point a)
{
    printf("%d %d\n", a.x, a.y);
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
    return Util_Sqrt(point.x * point.x + point.y * point.y);
}

Point Point_Normalize(const Point point, const int32_t normal)
{
    const int32_t magnitude = Point_Mag(point);
    if(magnitude == 0)
    {
        const Point huge = { INT_MAX, INT_MAX };
        return huge;
    }
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
