#include "Rect.h"

#include <stdlib.h>

// (-W/2, -H/2)
// +---------+
// |         |
// |         |
// |    +    |
// |         |
// |         |
// +---------+ (W/2, H/2)
Rect Rect_GetGridCenter(const Grid grid)
{
    const int32_t half_width = grid.tile_cart_width / 2;
    const int32_t half_height = grid.tile_cart_height / 2;
    const Rect rect = {
        { -half_width, -half_height }, { half_width, half_height }
    };
    return rect;
}

Rect Rect_GetEllipse(const Point center, const int32_t width)
{
    const Rect rect = {
        { center.x - width, center.y - (width / 2) },
        { center.x + width, center.y + (width / 2) },
    };
    return rect;
}

int32_t Rect_GetArea(const Rect rect)
{
    return abs(rect.b.x - rect.a.x) * abs(rect.b.y - rect.a.y);
}

// +---A
// | A | --+
// B---+   |
// B---+   |    A---+
// | B | --+--> |   |
// +---A   |    +---B
// +---B   |
// | C | --+
// A---+
Rect Rect_CorrectOrientation(const Rect rect)
{
    const Rect a = {
        { rect.b.x, rect.a.y },
        { rect.a.x, rect.b.y },
    };
    const Rect b = { rect.b, rect.a };
    const Rect c = { a.b, a.a };
    if(rect.a.x > rect.b.x && rect.a.y < rect.b.y) return a;
    if(rect.a.x > rect.b.x && rect.a.y > rect.b.y) return b;
    if(rect.a.x < rect.b.x && rect.a.y > rect.b.y) return c;
    return rect;
}

bool Rect_ContainsPoint(const Rect rect, const Point point)
{
    return point.x >= rect.a.x
        && point.y >= rect.a.y
        && point.x < rect.b.x
        && point.y < rect.b.y;
}

bool Rect_SeesRect(const Rect out, const Rect in)
{
    return Rect_ContainsPoint(out, in.a)
        || Rect_ContainsPoint(out, in.b);
}

// A-------------------+
// |                   |
// |       A------+    |
// |       |      |    |
// |       +------B    |
// |                   |
// +-------------------B
bool Rect_OnScreen(const Rect outline, const Rect rect)
{
    return outline.a.x >= rect.a.x
        && outline.a.y >= rect.a.y
        && outline.b.x < rect.b.x
        && outline.b.y < rect.b.y;
}

// A-------------------+
// |                   |
// |                   |
// |                   | A------+
// |                   | |      |
// |                   | +------B
// +-------------------B
bool Rect_TotallyOffScreen(const Rect outline, const Rect rect)
{
    return outline.a.x >= rect.b.x || outline.b.x < rect.a.x
        || outline.a.y >= rect.b.y || outline.b.y < rect.a.y;
}
