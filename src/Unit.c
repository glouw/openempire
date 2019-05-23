#include "Unit.h"

#include "Rect.h"
#include "Util.h"

static Point GetGlobalCoords(const Unit unit, const Grid grid)
{
    const Point mid = {
        grid.tile_cart_width  / 2,
        grid.tile_cart_height / 2,
    };
    const Point offset = {
        unit.cart_point.x * grid.tile_cart_width,
        unit.cart_point.y * grid.tile_cart_height,
    };
    const Point edge = Point_Add(mid, offset);
    return Point_Add(edge, unit.cart_fractional);
}

Unit Unit_Move(Unit unit, const Grid grid)
{
    const Point global = GetGlobalCoords(unit, grid);
    const Point delta = Point_Sub(global, unit.cart_goal);
    if(!Point_IsZero(delta))
    {
        // XXX. This velocity calculation is not perfect, but it gets the job done in terms of
        // moving a unit towards a single goal point.

        const int32_t min = UTIL_MIN(delta.x, delta.y);
        const Point velocity = Point_Div(delta, min);

        Point_Print(unit.cart_goal);
        Point_Print(global);
        Point_Print(velocity);
        putchar('\n');

        // Moving units is a little strange while in cartesian, as cartesian math is centered to the
        // middle of a cartesian tile. This was done to easy cartesian to isometric conversions.
        //
        // (-w/2, -h/2)
        // +---------+
        // |         |
        // |         |
        // |    +    |
        // |         |
        // |         |
        // +---------+ (w/2, h/2)

        unit.cart_fractional = Point_Sub(unit.cart_fractional, velocity);

        const int32_t hw = grid.tile_cart_width / 2;
        const int32_t hh = grid.tile_cart_height / 2;

        const Rect rect = {
            { -hw, -hh },
            { +hw, +hh },
        };

        // Nevertheless, the unit fractional coordinates (coordinates bound to the inside of the cartesian tile)
        // wrap around when a rect boundry is crossed and increments the cartesian map point by one in the direction
        // of the fractional crossing.

        const Point n = {  0, -1 };
        const Point s = {  0, +1 };
        const Point e = { +1,  0 };
        const Point w = { -1,  0 };

        if(unit.cart_fractional.x >= rect.b.x) unit.cart_fractional.x = rect.a.x - 0, unit.cart_point = Point_Add(unit.cart_point, e);
        if(unit.cart_fractional.y >= rect.b.y) unit.cart_fractional.y = rect.a.y - 0, unit.cart_point = Point_Add(unit.cart_point, s);
        if(unit.cart_fractional.x <  rect.a.x) unit.cart_fractional.x = rect.b.x - 1, unit.cart_point = Point_Add(unit.cart_point, w);
        if(unit.cart_fractional.y <  rect.a.y) unit.cart_fractional.y = rect.b.y - 1, unit.cart_point = Point_Add(unit.cart_point, n);
    }
    return unit;
}
