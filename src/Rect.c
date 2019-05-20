#include "Rect.h"

Rect Rect_Get(const Tile tile)
{
    const Point dimensions = {
        tile.frame.width,
        tile.frame.height
    };
    const Point a = {
        tile.iso_point.x + tile.iso_fractional.x - tile.frame.hotspot_x,
        tile.iso_point.y + tile.iso_fractional.y - tile.frame.hotspot_y,
    };
    const Point b = Point_Add(a, dimensions);
    const Rect rect = { a, b };
    return rect;
}
