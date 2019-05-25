#include "Rect.h"

// a---------+
// |         |
// |         |
// +---------b

Rect Rect_GetFrameOutline(const Tile tile)
{
    const Point dimensions = { tile.frame.width, tile.frame.height };
    const Point a = Tile_GetTopLeftCoords(tile);
    const Point b = Point_Add(a, dimensions);
    const Rect rect = { a, b };
    return rect;
}

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
