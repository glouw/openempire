#include "Units.h"

#include "Util.h"
#include "File.h"
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
        { {x_mid + 0, y_mid + 0}, {  0,   0}, false, FILE_FOREST_TREE            },
        { {x_mid + 0, y_mid + 0}, {  0,   0}, false, FILE_FOREST_TREE_SHADOW     },
        { {x_mid + 0, y_mid + 1}, {  0,   0}, false, FILE_BERRY_BUSH             },
        { {x_mid + 0, y_mid + 2}, {  0,   0}, false, FILE_STONE_MINE             },
        { {x_mid + 0, y_mid + 3}, {  0,   0}, false, FILE_GOLD_MINE              },
        { {x_mid - 2, y_mid + 0}, {-20, -20}, false, FILE_MALE_VILLAGER_STANDING },
        { {x_mid - 1, y_mid - 1}, {  0,   0}, false, FILE_MALE_VILLAGER_STANDING },
        { {x_mid - 1, y_mid - 1}, { 10,   0}, false, FILE_MALE_VILLAGER_STANDING },
        { {x_mid - 1, y_mid - 1}, {  0, -10}, false, FILE_MALE_VILLAGER_STANDING },
        { {x_mid - 1, y_mid - 1}, {  0, -13}, false, FILE_MALE_VILLAGER_STANDING },
        { {x_mid - 1, y_mid - 1}, {-10, -18}, false, FILE_MALE_VILLAGER_STANDING },
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

static Stack GetStackIso(const Units units, const Point iso, const Overview overview)
{
    const Point cart = Overview_IsoToCart(overview, iso, false);
    return Units_GetStackCart(units, cart);
}

void Units_Select(const Units units, const Overview overview, const Input input, const Registrar graphics)
{
    const Quad quad = Overview_GetRenderBox(overview, -200); // XXX, Border needs to be equal to largest building size.
    const Points points = Quad_GetRenderPoints(quad);
    const Tiles tiles = Tiles_PrepGraphics(graphics, overview, units, points); // XXX. A little excessive, as this is done in the renderer, but its gets the job done.
    if(input.lu)
    {
        // XXX. Must unselect all units with click.

        const Point click = { input.x, input.y };
        for(int32_t i = 0; i < tiles.count; i++)
        {
            const Tile tile = tiles.tile[i];
            if(Tile_ContainsPoint(tile, click))
            {
                // XXX. Only select unit if point clicked is not color key pixel.

                const Rect rect = Rect_Get(tile);

                const Point origin_click = Point_Sub(click, rect.a);

                if(Surface_GetPixel(tile.surface, origin_click.x, origin_click.y) != SURFACE_COLOR_KEY)
                {
                    // XXX. Must draw circle around selected unit.
                    puts("GOT EM");
                    tiles.unit[i]->selected = true;
                    break;
                }

            }
        }
    }
    Points_Free(points);
    Tiles_Free(tiles);
}

void Units_Command(const Units units, const Overview overview, const Input input)
{
    if(input.ru)
    {
        const Point click = {
            input.x,
            input.y,
        };

        const Point cart_raw = Overview_IsoToCart(overview, click, true);

        // Modulous by cartesian widths and heights to get the relative tile fractional offset.

        const Point cart_fractional = {
            cart_raw.x % overview.grid.tile_cart_width,
            cart_raw.y % overview.grid.tile_cart_height,
        };

        // Coordinate maths are done from tile center, so subtract tile mid point.

        const Point mid = {
            overview.grid.tile_cart_width / 2,
            overview.grid.tile_cart_height / 2,
        };

        const Point fixed = Point_Sub(cart_fractional, mid);

        printf("%d %d\n", fixed.x, fixed.y);
    }
}

static void ResetStacks(const Units units)
{
    for(int32_t y = 0; y < units.rows; y++)
    for(int32_t x = 0; x < units.cols; x++)
        units.stack[x + y * units.cols].count = 0;
}

static int32_t CompareByY(const void* a, const void* b)
{
    Unit* const aa = *((Unit**) a);
    Unit* const bb = *((Unit**) b);
    const Point pa = Point_ToIso(aa->cart_fractional);
    const Point pb = Point_ToIso(bb->cart_fractional);
    return pa.y < pb.y;
}

static void SortStacks(const Units units)
{
    for(int32_t y = 0; y < units.rows; y++)
    for(int32_t x = 0; x < units.cols; x++)
    {
        const Stack stack = units.stack[x + y * units.cols];
        if(stack.count > 1)
            qsort(stack.reference, stack.count, sizeof(*stack.reference), CompareByY);
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

void Units_Caretake(const Units units)
{
    ResetStacks(units);
    StackStacks(units);
    SortStacks(units);
}
