#include "Point.h"

#include <stdio.h>

bool Point_Equal(const Point a, const Point b)
{
    return a.x == b.x && a.y == b.y;
}

void Point_Print(const Point a)
{
    printf("%d %d\n", a.x, a.y);
}

Point Point_Add(const Point a, const Point b)
{
    const Point out = { a.x + b.x, a.y + b.y };
    return out;
}

Point Point_Sub(const Point a, const Point b)
{
    const Point out = { a.x - b.x, a.y - b.y };
    return out;
}
