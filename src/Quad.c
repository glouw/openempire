#include "Quad.h"

// SEE: HTTPS://STACKOVERFLOW.COM/QUESTIONS/2049582/HOW-TO-DETERMINE-IF-A-POINT-IS-IN-A-2D-TRIANGLE
static bool Inside(Point s, const Point a, const Point b, const Point c)
{
    const int32_t as_x = s.x - a.x;
    const int32_t as_y = s.y - a.y;
    const bool s_ab = (b.x - a.x) * as_y - (b.y - a.y) * as_x > 0;
    if(((c.x - a.x) * as_y - (c.y - a.y) * as_x > 0) == s_ab)
        return false;
    if(((c.x - b.x) * (s.y - b.y) - (c.y - b.y) * (s.x - b.x) > 0) != s_ab)
        return false;
    return true;
}

Points Quad_GetRenderPoints(const Quad quad)
{
    const int32_t boxed_area = (quad.b.x - quad.c.x) * (quad.d.y - quad.a.y);
    Points points = Points_New(boxed_area);
    for(int32_t y = quad.a.y; y < quad.d.y; y++)
    for(int32_t x = quad.c.x; x < quad.b.x; x++)
    {
        const Point point = { x, y };
        if(Inside(point, quad.a, quad.b, quad.d)
        || Inside(point, quad.a, quad.d, quad.c))
            points = Points_Append(points, point);
    }
    return points;
}
