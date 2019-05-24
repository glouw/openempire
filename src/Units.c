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
    const int32_t x_mid = units.cols / 2;
    const int32_t y_mid = units.rows / 2;
    const Unit test[] = {
        { { x_mid, y_mid + 1 }, { 0, 0 }, { NULL, 0, 0 }, 0, false, FILE_MALE_VILLAGER_STANDING },
        { { x_mid, y_mid - 1 }, { 0, 0 }, { NULL, 0, 0 }, 0, false, FILE_MALE_VILLAGER_STANDING },
        { { x_mid, y_mid - 1 }, { 0, 8 }, { NULL, 0, 0 }, 0, false, FILE_MALE_VILLAGER_STANDING },
        { { x_mid, y_mid - 1 }, { 8, 0 }, { NULL, 0, 0 }, 0, false, FILE_MALE_VILLAGER_STANDING },
        { { x_mid, y_mid - 2 }, { 0, 0 }, { NULL, 0, 0 }, 0, false, FILE_FOREST_TREE },
        { { x_mid, y_mid - 2 }, { 0, 0 }, { NULL, 0, 0 }, 0, false, FILE_FOREST_TREE_SHADOW },
    };
    for(int32_t i = 0; i < UTIL_LEN(test); i++)
        units = Units_Append(units, test[i]);
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
    if(input.lu)
    {
        UnSelectAll(units);
        const Point click = { input.x, input.y };
        Tiles_Select(tiles, click);
    }
    Points_Free(points);
    Tiles_Free(tiles);
}

static void ApplyPaths(const Units units, const Map map, const Point cart_point)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->selected)
        {
            const Field field = Field_New(map);
            Points_Free(unit->path);
            unit->path = Field_SearchBreadthFirst(field, unit->cart_point, cart_point);
            unit->path_index = 0;
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
        const Point cart_point = Overview_IsoToCart(overview, click, false);

        //const Point cart_raw = Overview_IsoToCart(overview, click, true);

        //// Modulous by cartesian widths and heights to get the relative tile fractional offset.

        //const Point cart_fractional = {
        //    cart_raw.x % overview.grid.tile_cart_width,
        //    cart_raw.y % overview.grid.tile_cart_height,
        //};

        //// Coordinate maths are done from tile center, so subtract tile mid point.

        //const Point mid = {
        //    overview.grid.tile_cart_width / 2,
        //    overview.grid.tile_cart_height / 2,
        //};
        //const Point fixed = Point_Sub(cart_fractional, mid);

        //printf("%d %d\n", cart_raw.x, cart_raw.y);
        //printf("%d %d\n", fixed.x, fixed.y);

        ApplyPaths(units, map, cart_point);
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

static void SortStacks(const Units units)
{
    for(int32_t y = 0; y < units.rows; y++)
    for(int32_t x = 0; x < units.cols; x++)
    {
        const Point point = { x, y };
        const Stack stack = *GetStack(units, point);
        Stack_Sort(stack);
    }
}

static void StackStacks(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        Stack* const stack = GetStack(units, unit->cart_point);
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
    SortStacks(units);
}
