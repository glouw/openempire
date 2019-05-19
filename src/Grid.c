#include "Grid.h"

#include "Point.h"

Grid Grid_Make(const int32_t cols, const int32_t rows, const int32_t tile_width, const int32_t tile_height)
{
    static Grid zero;
    Grid grid = zero;
    grid.cols = cols;
    grid.rows = rows;
    grid.tile_width = tile_width;
    grid.tile_height = tile_height;

    const Point iso_n = { 0, +tile_height / 2 };
    const Point iso_s = { 0, -tile_height / 2 };
    const Point iso_e = { +tile_width / 2, 0 };
    const Point a = Point_ToCart(iso_n);
    const Point b = Point_ToCart(iso_s);
    const Point c = Point_ToCart(iso_e);
    grid.cart_width = c.x - b.x;
    grid.cart_height = a.y - c.y;

    return grid;
}
