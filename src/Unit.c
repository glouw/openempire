#include "Unit.h"

#include "Rect.h"
#include "Util.h"

static Point GetCoords(const Grid grid, const Point cart, const Point local)
{
    const Point global = Grid_GetCoords(grid, cart);
    return Point_Add(global, local);
}

static Unit Velocitate(Unit unit, const Point delta, const Grid grid)
{
    const Point velocity = Point_Normalize(delta, unit.speed);
    unit.cell = Point_Add(unit.cell, velocity);
    unit.cart_grid_offset = Grid_CellToOffset(grid, unit.cell); // XXX. Unit movement is a little shakey. Can somehow integrate?
    unit.cart = Grid_CellToCart(grid, unit.cell);
    if(unit.selected)
        Point_Print(unit.cart);
    return unit;
}

Unit Unit_Move(Unit unit, const Grid grid)
{
    if(unit.path.count > 0)
    {
        if(unit.path.count >= 2
        && unit.path_index == 0)
            unit.path_index++;

        const Point unit_coords = GetCoords(grid, unit.cart, unit.cart_grid_offset);
        const Point goal_coords = GetCoords(grid, unit.path.point[unit.path_index], unit.cart_grid_offset_goal);
        const Point delta = Point_Sub(goal_coords, unit_coords);
        if(Point_Mag(delta) < 3)
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
    unit.cell = Grid_CartToCell(grid, cart);
    unit.cart = Grid_CellToCart(grid, unit.cell);
    unit.file = FILE_GRAPHICS_NONE;
    return unit;
}
