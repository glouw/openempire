#include "Unit.h"

#include "Rect.h"
#include "Util.h"

static Point GetCoords(const Grid grid, const Point cart, const Point local)
{
    const Point global = Grid_GetGridPoint(grid, cart);
    return Point_Add(global, local);
}

static Unit Velocitate(Unit unit, const Point delta, const Grid grid)
{
    const Point velocity = Point_Normalize(delta, unit.speed);
    unit.cell = Point_Add(unit.cell, velocity);
    unit.cart_grid_offset = Grid_CellToOffset(grid, unit.cell); // XXX. Unit movement is a little shakey. Can somehow integrate?
    unit.cart = Grid_CellToCart(grid, unit.cell);
    if(unit.selected)
        Point_Print(unit.cart_grid_offset);
    return unit;
}

Unit Unit_Move(Unit unit, const Grid grid)
{
    if(unit.path.count > 0)
    {
        // The first path goal is the middle of the current tile, which can be skipped.

        if(unit.path.count > 1 && unit.path_index == 0)
            unit.path_index++;

        const Point unit_grid_coords = GetCoords(grid, unit.cart, unit.cart_grid_offset);
        const Point zero = { 0, 0 };
        const Point select = (unit.path_index == unit.path.count - 1) ? unit.cart_grid_offset_goal : zero;
        const Point goal_grid_coords = GetCoords(grid, unit.path.point[unit.path_index], select);
        const Point delta = Point_Sub(goal_grid_coords, unit_grid_coords);
        if(Point_IsZero(delta))
        {
            unit.path_index++;
            if(unit.path_index == unit.path.count)
                unit.path = Points_Free(unit.path);
            return unit;
        }
        unit = Velocitate(unit, delta, grid);
    }
    return unit;
}

Unit Unit_Make(const Point cart, const Grid grid)
{
    static Unit zero;
    Unit unit = zero;
    unit.cart = cart;
    unit.cell = Grid_CartToCell(grid, unit.cart);
    unit.file = FILE_GRAPHICS_NONE;
    return unit;
}
