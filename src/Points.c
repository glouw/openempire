#include "Points.h"

#include "Util.h"

#include <assert.h>

Points Points_New(const int32_t max)
{
    Point* point = UTIL_ALLOC(Point, max);
    UTIL_CHECK(point);
    const Points points = { point, 0, max };
    return points;
}

Points Points_Free(Points points)
{
    free(points.point);
    points.point = NULL;
    points.count = 0;
    points.max = 0;
    return points;
}

Points Points_Append(Points points, const Point point)
{
    if(points.count == points.max)
    {
        points.max *= 2;
        Point* const temp = UTIL_REALLOC(points.point, Point, points.max);
        UTIL_CHECK(temp);
        points.point = temp;
    }
    points.point[points.count++] = point;
    return points;
}

Points Points_Cat(Points a, const Points b)
{
    const int32_t new_size = a.count + b.count;
    if(new_size > 0)
    {
        Point* const temp = UTIL_REALLOC(a.point, Point, new_size);
        UTIL_CHECK(temp);
        a.point = temp;
        a.max = new_size;
        for(int32_t i  = 0; i < b.count; i++)
            a = Points_Append(a, b.point[i]);
    }
    return a;
}

void Points_Print(const Points points)
{
    for(int32_t i = 0; i < points.count; i++)
        Point_Print(points.point[i]);
    fprintf(stderr, "\n");
}

static Points Drop(Points points, Point* point)
{
    assert(points.count >= 0);
    *point = points.point[--points.count];
    return points;
}

Points Points_Reverse(Points points)
{
    Points out = Points_New(points.count);
    for(int32_t i = 0; i < out.max; i++)
    {
        Point point;
        points = Drop(points, &point);
        out = Points_Append(out, point);
    }
    return out;
}
