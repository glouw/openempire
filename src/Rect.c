#include "Rect.h"

#include <stdlib.h>

// (-w/2, -h/2)
// +---------+
// |         |
// |         |
// |    +    |
// |         |
// |         |
// +---------+ (w/2, h/2)

Rect Rect_GetGridCenter(const Grid grid)
{
    const int32_t half_width = grid.tile_cart_width / 2;
    const int32_t half_height = grid.tile_cart_height / 2;
    const Rect rect = {
        { -half_width, -half_height }, { half_width, half_height }
    };
    return rect;
}

Rect Rect_GetEllipse(const Point center)
{
    const Rect rect = {
        { center.x - 20, center.y - 10 },
        { center.x + 20, center.y + 10 },
    };
    return rect;
}

int32_t Rect_GetArea(const Rect rect)
{
    return abs(rect.b.x - rect.a.x) * abs(rect.b.y - rect.a.y);
}

// +---a
// | A | --+
// b---+   |
// b---+   |    a---+
// | B | --+--> |   |
// +---a   |    +---b
// +---b   |
// | C | --+
// a---+

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
