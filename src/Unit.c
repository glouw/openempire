#include "Unit.h"

#include "Rect.h"
#include "Util.h"

// XXX. Needs collision detection for map edge and non-walkable objects (eg. tiles and units)
// Note that unit just needs to stop dead in their tracks - the current sweep will handle the rest.

static Unit Move(Unit unit, const Grid grid)
{
    unit.cell = Point_Add(unit.cell, unit.velocity);
    unit.cart_grid_offset = Grid_CellToOffset(grid, unit.cell);
    unit.cart = Grid_CellToCart(grid, unit.cell);
    return unit;
}

static Unit Accelerate(Unit unit, const Grid grid)
{
    if(unit.path.count > 0) // Accelerate if a path exists.
    {
        if(unit.path.count > 1 && unit.path_index == 0)
            unit.path_index++;
        static Point zero;
        const Point point = (unit.path_index == unit.path.count - 1) ? unit.cart_grid_offset_goal : zero;
        const Point goal_grid_coords = Grid_GetGridPointWithOffset(grid, unit.path.point[unit.path_index], point);
        const Point unit_grid_coords = Grid_GetGridPointWithOffset(grid, unit.cart, unit.cart_grid_offset);
        const Point delta = Point_Sub(goal_grid_coords, unit_grid_coords);
        if(Point_Mag(delta) < 10) // XXX: Is this too small? What about really fast moving guys?
        {
            unit.path_index++;
            if(unit.path_index == unit.path.count)
                unit.path = Points_Free(unit.path);
            return unit;
        }
        const Point dv = Point_Normalize(delta, unit.accel);
        unit.velocity = Point_Add(unit.velocity, dv);
    }
    else if(Point_Mag(unit.velocity) > 0) // Decelerate if there is no path.
    {
        const Point deccel = Point_Normalize(unit.velocity, unit.accel);
        const Point velocity = Point_Sub(unit.velocity, deccel);
        const Point dot = Point_Dot(velocity, unit.velocity);
        static Point zero;
        unit.velocity = (dot.x >= 0 && dot.y >= 0) ? velocity : zero; // XXX. Double check the math...
    }
    return unit;
}

static Unit CapSpeed(Unit unit)
{
    if(Point_Mag(unit.velocity) > unit.max_speed)
        unit.velocity = Point_Normalize(unit.velocity, unit.max_speed);
    return unit;
}

Unit Unit_MoveAlongPath(Unit unit, const Grid grid, const Point stressors)
{
    unit = Accelerate(unit, grid);
    unit.velocity = Point_Add(unit.velocity, stressors);
    unit = CapSpeed(unit);
    unit = Move(unit, grid);
    return unit;
}

Unit Unit_UpdateFile(Unit unit, const Graphics file)
{
    unit.max_speed = Graphics_GetMaxSpeed(file);
    unit.accel = Graphics_GetAcceleration(file);
    unit.file = file;
    unit.file_name = Graphics_GetString(file);
    return unit;
}

Unit Unit_Make(const Point cart, const Grid grid, const Graphics file)
{
    static Unit zero;
    Unit unit = zero;
    unit.cart = cart;
    unit.cell = Grid_CartToCell(grid, cart);
    unit = Unit_UpdateFile(unit, file);
    return unit;
}
