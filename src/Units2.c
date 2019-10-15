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
    units.unit[a].parent_id = units.unit[b].id;
    units.unit[b].has_children = true;
    return units;
}

Units Units_SpawnWithOffset(Units units, const Point cart, const Point offset, const Overview overview, const Graphics file, const Color color, const Registrar graphics)
{
    Unit unit = Unit_Make(cart, overview.grid, file, color, graphics);
    unit.cell = Point_Add(unit.cell, Grid_OffsetToCell(offset));
    return Append(units, unit);
}

static void LinkTailTownCenter(const Units units, const int32_t size)
{
    int32_t id = -1;
    for(int i = 0; i < size; i++)
    {
        Unit* const unit = &units.unit[size - 1 - i];
        if(i == 0)
        {
            unit->has_children = true;
            id = unit->id;
        }
        else unit->parent_id = id;
    }
}

Units Units_SpawnTownCenter(Units units, const Overview overview, const Registrar graphics, const Point cart, const Color color)
{
    const Point offset = { overview.grid.tile_cart_width / 2, -overview.grid.tile_cart_height / 2 };
    const Point zero = { 0,0 };
    struct
    {
        Point point;
        Point offset;
        Graphics file;
    }
    const layouts[] = {
        { {cart.x - 2, cart.y + 0}, zero,   FILE_DARK_AGE_TOWN_CENTER_SHADOW },
        { {cart.x - 3, cart.y + 1}, zero,   FILE_DARK_AGE_TOWN_CENTER_ROOF_LEFT },
        { {cart.x - 3, cart.y + 1}, offset, FILE_DARK_AGE_TOWN_CENTER_ROOF_LEFT_SUPPORT_A },
        { {cart.x - 2, cart.y + 0}, zero,   FILE_DARK_AGE_TOWN_CENTER_ROOF_LEFT_SUPPORT_B },
        { {cart.x - 3, cart.y + 1}, zero,   FILE_DARK_AGE_TOWN_CENTER_ROOF_RITE },
        { {cart.x - 3, cart.y + 1}, offset, FILE_DARK_AGE_TOWN_CENTER_ROOF_RITE_SUPPORT_A },
        { {cart.x - 2, cart.y + 0}, zero,   FILE_DARK_AGE_TOWN_CENTER_ROOF_RITE_SUPPORT_B },
        { {cart.x + 0, cart.y + 0}, zero,   FILE_DARK_AGE_TOWN_CENTER_TOP },
    };
    const int32_t size = UTIL_LEN(layouts);
    for(int i = 0; i < size; i++)
        units = Units_SpawnWithOffset(units, layouts[i].point, layouts[i].offset, overview, layouts[i].file, color, graphics);
    LinkTailTownCenter(units, size);
    return units;
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
