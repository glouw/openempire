#include "Units.h"

#include "Util.h"
#include "File.h"
#include "Field.h"
#include "Rect.h"
#include "Surface.h"
#include "Tiles.h"
#include "Graphics.h"

#include <stdlib.h>

static Units GenerateTestZone(Units units)
{
    for(int32_t i = 0; i < 50; i++)
    {
        const Point cart = {
            Util_Rand() % units.cols,
            Util_Rand() % units.rows,
        };
        Unit unit = Unit_Make(cart);
        Unit shadow = Unit_Make(cart);
        switch(Util_Rand() % 5)
        {
        default:
        case 0:
            unit.file = FILE_MALE_VILLAGER_STANDING;
            break;
        case 1:
            unit.file = FILE_FOREST_TREE;
            shadow.file = FILE_FOREST_TREE_SHADOW;
            break;
        case 2:
            unit.file = FILE_GOLD_MINE;
            break;
        case 3:
            unit.file = FILE_STONE_MINE;
            break;
        case 4:
            unit.file = FILE_BERRY_BUSH;
            break;
        }
        units = Units_Append(units, unit);
        if(shadow.file != FILE_GRAPHICS_NONE)
            units = Units_Append(units, shadow);
    }
    return units;
}

Units Units_New(const int32_t max, const int32_t rows, const int32_t cols)
{
    const int32_t area = rows * cols;
    Unit* const unit = UTIL_ALLOC(Unit, max);
    Stack* const stack = UTIL_ALLOC(Stack, area);
    UTIL_CHECK(unit);
    UTIL_CHECK(stack);
    for(int32_t i = 0; i < area; i++)
        stack[i] = Stack_Build(8);
    Units units = { unit, 0, max, stack, rows, cols };
    units = GenerateTestZone(units);
    return units;
}

Units Units_Append(Units units, const Unit unit)
{
    if(units.count == units.max)
    {
        units.max *= 2;
        Unit* const temp = UTIL_REALLOC(units.unit, Unit, units.max);
        UTIL_CHECK(temp);
        units.unit = temp;
    }
    units.unit[units.count++] = unit;
    return units;
}

void Units_Free(const Units units)
{
    const int32_t area = units.rows * units.cols;
    for(int32_t i = 0; i < area; i++)
        Stack_Free(units.stack[i]);
    free(units.stack);
    free(units.unit);
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
    return &units.stack[p.x + p.y * units.cols]; // Implies cartesian.
}

Stack Units_GetStackCart(const Units units, const Point p)
{
    static Stack zero;
    return OutOfBounds(units, p) ? zero : *GetStack(units, p); // Return a shallow copy of the stack to prevent write access.
}

static void UnSelectAll(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].selected = false;
}

void Units_SelectOne(const Units units, const Overview overview, const Input input, const Registrar graphics)
{
    const Quad quad = Overview_GetRenderBox(overview, -200); // XXX, Border needs to be equal to largest building size.
    const Points points = Quad_GetRenderPoints(quad);
    const Tiles tiles = Tiles_PrepGraphics(graphics, overview, units, points); // XXX. A little excessive, as this is done in the renderer, but its gets the job done.
    const Point click = { input.x, input.y };
    if(input.lu)
    {
        UnSelectAll(units);
        if(Overview_IsSelectionBoxBigEnough(overview))
            Tiles_SelectMany(tiles, overview.selection_box);
        else
            Tiles_SelectOne(tiles, click);
    }
    Points_Free(points);
    Tiles_Free(tiles);
}

static void ApplyPathsToSelected(const Units units, const Map map, const Point cart_goal, const Point cart_fractional_local_goal)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->selected)
        {
            const Field field = Field_New(map, units); // XXX. Should really only construct this once, unless the map is always changing?
            Points_Free(unit->path);
            unit->path = Field_SearchBreadthFirst(field, unit->cart, cart_goal);
            unit->path_index = 0;
            unit->cart_fractional_local_goal = cart_fractional_local_goal;
            Field_Free(field);
            Points_Print(unit->path);
        }
    }
}

void Units_Command(const Units units, const Overview overview, const Input input, const Map map)
{
    if(input.ru)
    {
        const Point click = { input.x, input.y };
        const Point cart_goal = Overview_IsoToCart(overview, click, false);
        const Point cart_global = Overview_IsoToCart(overview, click, true);
        const Point cart_fractional_local_goal = Grid_GetLocalCoords(overview.grid, cart_global);
        if(Units_CanWalk(units, map, cart_goal))
            ApplyPathsToSelected(units, map, cart_goal, cart_fractional_local_goal);
    }
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
        Stack* const stack = GetStack(units, unit->cart);
        Stack_Append(stack, unit);
    }
}

static void Move(const Units units, const Grid grid)
{
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i] = Unit_Move(units.unit[i], grid);
}

void Units_Caretake(const Units units, const Grid grid)
{
    Move(units, grid);
    ResetStacks(units);
    StackStacks(units);
}

bool Units_CanWalk(const Units units, const Map map, const Point point)
{
    const Terrain terrain = Map_GetTerrainFile(map, point);
    const Stack stack = Units_GetStackCart(units, point);
    return stack.reference && Terrain_IsWalkable(terrain) && Stack_IsWalkable(stack);
}
