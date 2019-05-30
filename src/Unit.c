#include "Unit.h"

#include "Rect.h"
#include "Util.h"

#include <limits.h>

static Point GetGlobalCart(const Grid grid, const Point cart, const Point local)
{
    const Point global = Grid_GetCoords(grid, cart);
    return Point_Add(global, local);
}

static Unit Velocitate(Unit unit, const Point delta, const Grid grid)
{
    const int32_t mag = Point_Mag(delta);
    if(mag == 0)
        return unit;
    const Point expanded = Point_Mul(delta, unit.speed);
    const Point velocity = Point_Div(expanded, mag);
    if(unit.selected)
        printf("-->>%d\n", Point_Mag(velocity));
    unit.cell = Point_Add(unit.cell, velocity);
    unit.cart_grid_offset = Grid_CellToOffset(grid, unit.cell); // XXX. Unit movement is a little shakey. Can somehow integrate?
    unit.cart = Grid_CellToCart(grid, unit.cell);
    return unit;
}

Unit Unit_Move(Unit unit, const Grid grid)
{
    if(unit.path.count > 0)
    {
        const Point unit_global = GetGlobalCart(grid, unit.cart, unit.cart_grid_offset);
        const Point goal_global = GetGlobalCart(grid, unit.path.point[unit.path.count - 1], unit.cart_grid_offset_goal);
        const Point delta = Point_Sub(goal_global, unit_global);
        if(Point_Mag(delta) < 2)
        {
            //unit.path_index++;
            //if(unit.path_index == unit.path.count)
            //    unit.path = Points_Free(unit.path);

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
