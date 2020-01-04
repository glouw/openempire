// General purpose memory management.

#include "Units.h"

#include "Util.h"

static Units Append(Units units, const Unit unit)
{
    if(units.count == units.max)
        Util_Bomb("OUT OF MEMORY\n");
    units.unit[units.count++] = unit;
    return units;
}

static Units BulkAppend(Units units, const Map map, Unit unit[], const int32_t len, const bool ignore_collisions)
{
    if(!ignore_collisions)
        for(int32_t i = 0; i < len; i++)
            if(!Units_CanBuild(units, map, &unit[i]))
                return units;
    for(int32_t i = 0; i < len; i++)
        units = Append(units, unit[i]);
    return units;
}

void SetChildren(Unit unit[], const int32_t count)
{
    if(count > 1)
    {
        unit[0].has_children = true;
        for(int32_t i = 1; i < count; i++)
            unit[i].parent_id = unit[0].id;
    }
}

Units Units_SpawnParts(Units units, const Point cart, const Point offset, const Grid grid, const Color color, const Registrar graphics, const Map map, const bool is_floating, const Parts parts, const bool ignore_collisions)
{
    Unit* const temp = UTIL_ALLOC(Unit, parts.count);
    for(int32_t i = 0; i < parts.count; i++)
    {
        const Part part = parts.part[i];
        const Point cart_part = Point_Add(cart, part.cart);
        temp[i] = Unit_Make(cart_part, offset, grid, part.file, color, graphics, true, is_floating, part.trigger);
    }
    SetChildren(temp, parts.count);
    units = BulkAppend(units, map, temp, parts.count, ignore_collisions);
    free(temp);
    return units;
}

void Units_ResetTiled(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].is_already_tiled = false;
}

static bool OutOfBounds(const Units units, const Point point)
{
    return point.x < 0 || point.y < 0 || point.x >= units.cols || point.y >= units.rows;
}

static Stack* GetStack(const Units units, const Point p)
{
    return &units.stack[p.x + p.y * units.cols];
}

Stack Units_GetStackCart(const Units units, const Point p)
{
    static Stack zero;
    return OutOfBounds(units, p) ? zero : *GetStack(units, p);
}

void Units_ResetStacks(const Units units)
{
    for(int32_t y = 0; y < units.rows; y++)
    for(int32_t x = 0; x < units.cols; x++)
    {
        const Point point = { x, y };
        GetStack(units, point)->count = 0;
    }
}

static void SafeAppend(const Units units, Unit* const unit, const Point cart)
{
    if(!OutOfBounds(units, cart))
        Stack_Append(GetStack(units, cart), unit);
}

void Units_StackStacks(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->trait.is_inanimate)
            for(int32_t y = 0; y < unit->trait.dimensions.y; y++)
            for(int32_t x = 0; x < unit->trait.dimensions.x; x++)
            {
                const Point point = { x, y };
                const Point cart = Point_Add(point, unit->cart);
                SafeAppend(units, unit, cart);
            }
        else SafeAppend(units, unit, unit->cart);
    }
}

void Units_ManageStacks(const Units units)
{
    Units_ResetStacks(units);
    Units_StackStacks(units);
}
