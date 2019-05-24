#include "Unit.h"

#include "Rect.h"
#include "Util.h"

static Point GetGlobalCoords(const Unit unit, const Grid grid)
{
    const Point grid_global = Grid_GetGlobalCoords(grid, unit.cart);
    return Point_Add(grid_global, unit.cart_fractional_local);
}

Unit Unit_Move(Unit unit, const Grid grid)
{
    if(unit.path.count > 0)
    {
        const Point unit_global = GetGlobalCoords(unit, grid);
        const Point goal_global = Grid_GetGlobalCoords(grid, unit.path.point[unit.path_index]);
        Point delta = Point_Sub(unit_global, goal_global);

        // Given this is the last point of the path, zero in on the fractional offset.

        if(unit.path_index == unit.path.count - 1)
            delta = Point_Sub(delta, unit.cart_fractional_local_goal);
        Point_Print(delta);

        // Reaching the end of a point in a single frame isn't much use.
        // The next path must be indexed, and movement must be started in this frame,
        // else the sprite will flicker between path indices.

        if(Point_IsZero(delta))
        {
            unit.path_index++;
            if(unit.path_index == unit.path.count)
                unit.path = Points_Free(unit.path);
            return Unit_Move(unit, grid);
        }

        // XXX. This velocity calculation is not perfect, but it gets the job done.
        // What is required is a higher resolution integer vector system, else diagonal movements will
        // always be about 41.2% faster than horizontal and vertical movements.

        const int32_t min = UTIL_MIN(abs(delta.x), abs(delta.y));
        const int32_t max = UTIL_MAX(abs(delta.x), abs(delta.y));
        const int32_t pick = (min == 0) ? max : min;
        const Point direction = Point_Div(delta, pick);

        // Moving units is a little strange while in cartesian, as cartesian math is centered to the
        // middle of a cartesian tile. This was done to ease cartesian to isometric conversions.
        //
        // (-w/2, -h/2)
        // +---------+
        // |         |
        // |         |
        // |    +    |
        // |         |
        // |         |
        // +---------+ (w/2, h/2)

        unit.cart_fractional_local = Point_Sub(unit.cart_fractional_local, direction);

        const int32_t hw = grid.tile_cart_width / 2;
        const int32_t hh = grid.tile_cart_height / 2;

        const Rect rect = { { -hw, -hh }, { +hw, +hh } };

        // Nevertheless, the unit fractional coordinates (coordinates bound to the inside of the cartesian tile)
        // wrap around when a rect boundry is crossed and increments the cartesian map point by one in the direction
        // of the fractional crossing.

        const Point n = {  0, -1 };
        const Point s = {  0, +1 };
        const Point e = { +1,  0 };
        const Point w = { -1,  0 };

        if(unit.cart_fractional_local.x >= rect.b.x) unit.cart_fractional_local.x = rect.a.x - 0, unit.cart = Point_Add(unit.cart, e);
        if(unit.cart_fractional_local.y >= rect.b.y) unit.cart_fractional_local.y = rect.a.y - 0, unit.cart = Point_Add(unit.cart, s);
        if(unit.cart_fractional_local.x <  rect.a.x) unit.cart_fractional_local.x = rect.b.x - 1, unit.cart = Point_Add(unit.cart, w);
        if(unit.cart_fractional_local.y <  rect.a.y) unit.cart_fractional_local.y = rect.b.y - 1, unit.cart = Point_Add(unit.cart, n);
    }
    return unit;
}
