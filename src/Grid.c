#include "Grid.h"

#include "Point.h"
#include "Config.h"

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
    grid.tile_cart_mid.x = grid.tile_cart_width / 2;
    grid.tile_cart_mid.y = grid.tile_cart_height / 2;
    return grid;
}

Point Grid_GetGridPoint(const Grid grid, const Point point)
{
    const Point out = {
        point.x * grid.tile_cart_width,
        point.y * grid.tile_cart_height,
    };
    return Point_Add(out, grid.tile_cart_mid);
}

Point Grid_GetGridPointWithOffset(const Grid grid, const Point cart, const Point offset)
{
    const Point global = Grid_GetGridPoint(grid, cart);
    return Point_Add(global, offset);
}

Point Grid_GetOffsetFromGridPoint(const Grid grid, const Point point)
{
    const Point out = {
        point.x % grid.tile_cart_width,
        point.y % grid.tile_cart_height,
    };
    return Point_Sub(out, grid.tile_cart_mid);
}

Point Grid_CellToCart(const Grid grid, const Point cell)
{
    const Point out = {
        (cell.x / CONFIG_GRID_CELL_SIZE) / grid.tile_cart_width,
        (cell.y / CONFIG_GRID_CELL_SIZE) / grid.tile_cart_height,
    };
    return out;
}

Point Grid_CartToCell(const Grid grid, const Point cart)
{
    const int32_t w = CONFIG_GRID_CELL_SIZE * grid.tile_cart_width;
    const int32_t h = CONFIG_GRID_CELL_SIZE * grid.tile_cart_height;
    const Point out = {
        w * cart.x + w / 2,
        h * cart.y + h / 2,
    };
    return out;
}

Point Grid_CellToOffset(const Grid grid, const Point cell)
{
    const Point coords = {
        (cell.x / CONFIG_GRID_CELL_SIZE) % grid.tile_cart_width,
        (cell.y / CONFIG_GRID_CELL_SIZE) % grid.tile_cart_height,
    };
    return Point_Sub(coords, grid.tile_cart_mid);
}

Point Grid_GetCornerOffset(const Grid grid, const Point offset)
{
    return Point_Add(offset, grid.tile_cart_mid);
}

Point Grid_OffsetToCell(const Point offset)
{
    return Point_Mul(offset, CONFIG_GRID_CELL_SIZE);
}

static Point GetCenterTile(const Grid grid)
{
    const Point out = {
        (grid.cols * grid.tile_cart_width) / 2,
        (grid.rows * grid.tile_cart_height) / 2,
    };
    return out;
}

Point Grid_PanToCart(const Grid grid, const Point pan)
{
    const Point cart = Point_ToCart(pan);
    const Point center = GetCenterTile(grid);
    const Point shift = Point_Add(cart, center);
    const Point out = {
        shift.x / grid.tile_cart_width,
        shift.y / grid.tile_cart_height,
    };
    return out;
}

Point Grid_CartToPan(const Grid grid, const Point cart)
{
    const Point out = {
        cart.x * grid.tile_cart_width,
        cart.y * grid.tile_cart_height,
    };
    const Point center = GetCenterTile(grid);
    const Point shift = Point_Sub(out, center);
    return Point_ToIso(shift);
}
