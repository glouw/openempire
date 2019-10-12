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

Units Units_Spawn(Units units, const Point cart, const Grid grid, const Graphics file, const Color color, const Registrar graphics)
{
    return Append(units, Unit_Make(cart, grid, file, color, graphics));
}

Units Units_SpawnWithShadow(Units units, const Point cart, const Grid grid, const Graphics file, const Color color, const Registrar graphics, const Graphics shadow)
{
    units = Units_Spawn(units, cart, grid, file, color, graphics);
    units = Units_Spawn(units, cart, grid, shadow, color, graphics);
    const int32_t a = units.count - 1;
    const int32_t b = units.count - 2;
    units.unit[b].shadow_id = units.unit[a].id;
    units.unit[b].has_shadow = true;
    return units;
}

Units Units_SpawnWithOffset(Units units, const Point cart, const Point offset, const Overview overview, const Graphics file, const Color color, const Registrar graphics)
{
    Unit unit = Unit_Make(cart, overview.grid, file, color, graphics);
    unit.cell = Point_Add(unit.cell, Grid_OffsetToCell(offset));
    return Append(units, unit);
}

void Units_ResetTiled(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].already_tiled = false;
}

static bool OutOfBounds(const Units units, const Point point)
{
    return point.x < 0
        || point.y < 0
        || point.x >= units.cols
        || point.y >= units.rows;
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

static void ResetStacks(const Units units)
{
    for(int32_t y = 0; y < units.rows; y++)
    for(int32_t x = 0; x < units.cols; x++)
    {
        const Point point = { x, y };
        GetStack(units, point)->count = 0;
    }
}

static void StackStacks(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->trait.is_building)
            for(int32_t y = 0; y < unit->trait.dimensions.y; y++)
            for(int32_t x = 0; x < unit->trait.dimensions.x; x++)
            {
                const Point point = { x, y };
                const Point cart = Point_Add(point, unit->cart);
                Stack* const stack = GetStack(units, cart);
                Stack_Append(stack, unit);
            }
        else
        {
            Stack* const stack = GetStack(units, unit->cart);
            Stack_Append(stack, unit);
        }
    }
}

static void CalculateCenters(const Units units)
{
    for(int32_t y = 0; y < units.rows; y++)
    for(int32_t x = 0; x < units.cols; x++)
    {
        const Point point = { x, y };
        Stack* const stack = GetStack(units, point);
        Stack_UpdateCenterOfMass(stack);
    }
}

void Units_ManageStacks(const Units units)
{
    ResetStacks(units);
    StackStacks(units);
    CalculateCenters(units);
}
