#include "Grid.h"

#include "Point.h"

Grid Grid_Make(const int32_t cols, const int32_t rows, const int32_t tile_iso_width, const int32_t tile_iso_height)
{
    static Grid zero;
    Grid grid = zero;
    grid.cols = cols;
    grid.rows = rows;
    grid.tile_iso_width = tile_iso_width;
    grid.tile_iso_height = tile_iso_height;

    const Point iso_n = { 0, -tile_iso_height / 2 }; // n
    const Point iso_s = { 0, +tile_iso_height / 2 }; //   e
    const Point iso_e = { +tile_iso_width / 2, 0 };  // s
    const Point a = Point_ToCart(iso_n); //     a
    const Point b = Point_ToCart(iso_s); //     |
    const Point c = Point_ToCart(iso_e); // b - c

    grid.tile_cart_width = c.x - b.x;
    grid.tile_cart_height = c.y - a.y;

    // Dictates with width and height of sub-pixels within a pixel.

    grid.cell_size = 1000;

    return grid;
}

Point Grid_GetCoords(const Grid grid, const Point point)
{
    const Point mid = {
        grid.tile_cart_width  / 2,
        grid.tile_cart_height / 2,
    };
    const Point out = {
        point.x * grid.tile_cart_width,
        point.y * grid.tile_cart_height,
    };
    return Point_Add(out, mid);
}

Point Grid_GetOffset(const Grid grid, const Point point)
{
    const Point mid = {
        grid.tile_cart_width / 2,
        grid.tile_cart_height / 2,
    };
    const Point out = {
        point.x % grid.tile_cart_width,
        point.y % grid.tile_cart_height,
    };
    return Point_Sub(out, mid);
}

Point Grid_CellToOffset(const Grid grid, const Point cell)
{
    const Point out = {
        (cell.x / grid.cell_size) % grid.tile_cart_width,
        (cell.y / grid.cell_size) % grid.tile_cart_height,
    };
    return out;
}

Point Grid_CellToCart(const Grid grid, const Point cell)
{
    const Point out = {
        (cell.x / grid.cell_size) / grid.tile_cart_width,
        (cell.y / grid.cell_size) / grid.tile_cart_height,
    };
    return out;
}

Point Grid_CartToCell(const Grid grid, const Point cart)
{
    const Point out = {
        cart.x * grid.cell_size * grid.tile_cart_width,
        cart.y * grid.cell_size * grid.tile_cart_height,
    };
    return out;
}
