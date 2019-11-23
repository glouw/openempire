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

Units Units_Spawn(Units units, const Point cart, const Point offset, const Grid grid, const Graphics file, const Color color, const Registrar graphics, const Map map, const bool is_floating)
{
    Unit unit = Unit_Make(cart, offset, grid, file, color, graphics, true, is_floating);
    return Units_CanBuild(units, map, &unit)
        ? Append(units, unit)
        : units;
}

static Units BulkAppend(Units units, const Map map, Unit unit[], const int32_t len)
{
    for(int32_t i = 0; i < len; i++)
        if(!Units_CanBuild(units, map, &unit[i]))
            return units;
    for(int32_t i = 0; i < len; i++)
        units = Append(units, unit[i]);
    return units;
}

Units Units_SpawnWithChild(Units units, const Point cart, const Grid grid, const Graphics file, const Color color, const Registrar graphics, const Graphics child, const Map map, const bool is_floating)
{
    const Point offset = { 0,0 };
    Unit temp[] = {
        Unit_Make(cart, offset, grid, file,  color, graphics, true, is_floating),
        Unit_Make(cart, offset, grid, child, color, graphics, true, is_floating),
    };
    temp[0].has_children = true;
    temp[1].parent_id = temp[0].id;
    return BulkAppend(units, map, temp, UTIL_LEN(temp));
}

Units Units_SpawnTownCenter(Units units, const Point cart, const Grid grid, const Color color, const Registrar graphics, const Map map, const bool is_floating)
{
    const Point offset = {
        +grid.tile_cart_mid.x,
        -grid.tile_cart_mid.y
    };
    const Point zero = { 0,0 };
    const Point b = { cart.x + 0, cart.y + 0 };
    const Point c = { cart.x - 1, cart.y + 1 };
    const Point a = { cart.x - 3, cart.y + 2 };
    const Point d = { cart.x - 1, cart.y + 1 };
    const Point e = { cart.x - 2, cart.y + 2 };
    Unit temp[] = {
        Unit_Make(b, zero,   grid, FILE_DARK_AGE_TOWN_CENTER_TOP,                 color, graphics, true,  is_floating),
        Unit_Make(c, zero,   grid, FILE_DARK_AGE_TOWN_CENTER_SHADOW,              color, graphics, true,  is_floating),
        Unit_Make(a, zero,   grid, FILE_DARK_AGE_TOWN_CENTER_ROOF_LEFT,           color, graphics, false, is_floating),
        Unit_Make(e, offset, grid, FILE_DARK_AGE_TOWN_CENTER_ROOF_LEFT_SUPPORT_A, color, graphics, false, is_floating),
        Unit_Make(d, zero,   grid, FILE_DARK_AGE_TOWN_CENTER_ROOF_LEFT_SUPPORT_B, color, graphics, false, is_floating),
        Unit_Make(a, zero,   grid, FILE_DARK_AGE_TOWN_CENTER_ROOF_RITE,           color, graphics, false, is_floating),
        Unit_Make(e, offset, grid, FILE_DARK_AGE_TOWN_CENTER_ROOF_RITE_SUPPORT_A, color, graphics, false, is_floating),
        Unit_Make(d, zero,   grid, FILE_DARK_AGE_TOWN_CENTER_ROOF_RITE_SUPPORT_B, color, graphics, false, is_floating),
    };
    const int32_t len = UTIL_LEN(temp);
    temp[0].has_children = true;
    for(int32_t i = 1; i < len; i++)
        temp[i].parent_id = temp[0].id;
    return BulkAppend(units, map, temp, len);
}

void Units_ResetTiled(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].already_tiled = false;
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
