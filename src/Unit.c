#include "Unit.h"

#include "Rect.h"
#include "Util.h"

static Point GetGlobalCart(const Unit unit, const Grid grid)
{
    const Point global = Grid_GetGlobalCoords(grid, unit.cart);
    return Point_Add(global, unit.cart_fractional_local);
}

static Point GetGlobalCartGoal(const Unit unit, const Grid grid)
{
    Point global = Grid_GetGlobalCoords(grid, unit.path.point[unit.path_index]);

    // If in the last square, zero in on fractional offset.

    if(unit.path_index == unit.path.count - 1)
        global = Point_Add(global, unit.cart_fractional_local_goal);
    return global;
}

static Unit Step(Unit unit, const Point direction, const Grid grid)
{
    const Point n = {  0, -1 };
    const Point s = {  0, +1 };
    const Point e = { +1,  0 };
    const Point w = { -1,  0 };
    const Rect rect = Rect_GetGridCenter(grid);

    unit.cart_fractional_local = Point_Sub(unit.cart_fractional_local, direction);

    if(unit.cart_fractional_local.x >= rect.b.x) unit.cart_fractional_local.x = rect.a.x - 0, unit.cart = Point_Add(unit.cart, e);
    if(unit.cart_fractional_local.y >= rect.b.y) unit.cart_fractional_local.y = rect.a.y - 0, unit.cart = Point_Add(unit.cart, s);
    if(unit.cart_fractional_local.x <  rect.a.x) unit.cart_fractional_local.x = rect.b.x - 1, unit.cart = Point_Add(unit.cart, w);
    if(unit.cart_fractional_local.y <  rect.a.y) unit.cart_fractional_local.y = rect.b.y - 1, unit.cart = Point_Add(unit.cart, n);

    return unit;
}

Unit Unit_Move(Unit unit, const Grid grid)
{
    if(unit.path.count > 0)
    {
        // Skip moving to the middle of the first square.

        if(unit.path_index == 0 && unit.path.count > 1)
            unit.path_index++;

        const Point unit_global = GetGlobalCart(unit, grid);
        const Point goal_global = GetGlobalCartGoal(unit, grid);

        // Reaching the end of a point will result in a delta of zero,
        // which yields no velocity information. This will drop one frame and cause the unit to flicker
        // as it crosses grid boundries while the path index increments...

        const Point delta = Point_Sub(unit_global, goal_global);
        if(Point_IsZero(delta))
        {
            unit.path_index++;
            if(unit.path_index == unit.path.count)
                unit.path = Points_Free(unit.path);

            // ... That being said, the best solution is to run the next movement in this current frame.

            return Unit_Move(unit, grid);
        }

        // XXX. This velocity calculation is not perfect, but it gets the job done for now so I can move onto other things.
        // What is required is a higher resolution integer vector system, else diagonal movements will
        // always be about 41.2% faster than horizontal and vertical movements.

        const int32_t divisor = UTIL_MAX(abs(delta.x), abs(delta.y));
        const Point velocity = Point_Div(delta, divisor);
        unit = Step(unit, velocity, grid);
    }
    return unit;
}
