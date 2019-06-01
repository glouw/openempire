#include "Unit.h"

#include "Rect.h"
#include "Util.h"

Unit Unit_Move(Unit unit, const Grid grid) // XXX. Needs a point for external stressors from other units to add to velocity.
{
    // Given a path exists, accelerate the unit along the path.

    if(unit.path.count > 0)
    {
        // Given more than one path index exists, the first index can be skippe as it points to the middle of the first tile.

        if(unit.path.count > 1 && unit.path_index == 0)
            unit.path_index++;

        // The last point index accounts for the grid offset, otherwise, as the last point index is approached, tiles
        // are stepped towards the middle.

        const Point zero = { 0, 0 };
        const Point select = (unit.path_index == unit.path.count - 1) ? unit.cart_grid_offset_goal : zero;
        const Point goal_grid_coords = Grid_GetGridPointWithOffset(grid, unit.path.point[unit.path_index], select);
        const Point unit_grid_coords = Grid_GetGridPointWithOffset(grid, unit.cart, unit.cart_grid_offset);
        const Point delta = Point_Sub(goal_grid_coords, unit_grid_coords);
        if(Point_Mag(delta) < 5) // XXX: Is this too small? What about really fast moving guys?
        {
            unit.path_index++;
            if(unit.path_index == unit.path.count)
                unit.path = Points_Free(unit.path);
            return unit;
        }

        // Apply velocity.

        const Point dv = Point_Normalize(delta, unit.accel);
        unit.velocity = Point_Add(unit.velocity, dv);

        // Check max speed.

        if(Point_Mag(unit.velocity) > unit.max_speed)
            unit.velocity = Point_Normalize(unit.velocity, unit.max_speed);
    }
    else if(Point_Mag(unit.velocity) > 0) // If no path exists for the unit, slow down the unit.
    {
        const Point deccel = Point_Normalize(unit.velocity, unit.accel);
        unit.velocity = Point_Sub(unit.velocity, deccel); // XXX: WHAT IF UNIT STARTS ACCELERATING BACKWARDS BECAUSE NOT EXACT 0?
    }

    // Apply velocity to position.

    unit.cell = Point_Add(unit.cell, unit.velocity);
    unit.cart_grid_offset = Grid_CellToOffset(grid, unit.cell); // XXX. Unit movement is a little shakey. Can somehow integrate?
    unit.cart = Grid_CellToCart(grid, unit.cell);

    return unit;
}

Unit Unit_UpdateFile(Unit unit, const Graphics file)
{
    unit.max_speed = Graphics_GetMaxSpeed(file);
    unit.accel = Graphics_GetAcceleration(file);
    unit.file = file;
    unit.file_name = Graphics_GetString(file);
    printf("%d %d %s\n", unit.max_speed, unit.accel, unit.file_name);
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
