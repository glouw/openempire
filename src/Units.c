#include "Units.h"

#include "Util.h"
#include "File.h"
#include "Graphics.h"

#include <stdlib.h>

static Units GenerateTestZone(Units units)
{
    const int32_t x_mid = units.cols / 2;
    const int32_t y_mid = units.rows / 2;
    const Unit test[] = {
        { {x_mid + 0, y_mid + 0}, {  0,   0}, FILE_FOREST_TREE },
        { {x_mid + 0, y_mid + 0}, {  0,   0}, FILE_FOREST_TREE_SHADOW },
        { {x_mid + 0, y_mid + 1}, {  0,   0}, FILE_BERRY_BUSH },
        { {x_mid + 0, y_mid + 2}, {  0,   0}, FILE_STONE_MINE },
        { {x_mid + 0, y_mid + 3}, {  0,   0}, FILE_GOLD_MINE },

        // XXX: Our boys in blue need to be sorted.
        // Do this before the mouse cursor is used for selecting units
        // (eg. dont use base tile for selecting units)

        { {x_mid - 1, y_mid - 1}, {+20, +20}, FILE_MALE_VILLAGER_STANDING },
        { {x_mid - 1, y_mid - 1}, {-20, +20}, FILE_MALE_VILLAGER_STANDING },
        { {x_mid - 1, y_mid - 1}, {+20, -20}, FILE_MALE_VILLAGER_STANDING },
        { {x_mid - 1, y_mid - 1}, {  0,   0}, FILE_MALE_VILLAGER_STANDING },
        { {x_mid - 1, y_mid - 1}, {-20, -20}, FILE_MALE_VILLAGER_STANDING },
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
    const Point cart = Overview_IsoToCart(overview, iso);
    return Units_GetStackCart(units, cart);
}

void Units_Select(const Units units, const Overview overview, const Input input)
{
    if(input.lu)
    {
        const Point click = { input.x, input.y };
        const Stack stack = GetStackIso(units, click, overview);
        for(int32_t i = 0; i < stack.count; i++)
            printf("%d: %s\n", i, Graphics_GetString(stack.reference[i]->file));
        putchar('\n');
    }
}

static void StackUnits(const Units units)
{
    for(int32_t y = 0; y < units.rows; y++)
    for(int32_t x = 0; x < units.cols; x++)
        units.stack[x + y * units.cols].count = 0;

    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        Stack* const stack = GetStack(units, unit->cart_point);
        Stack_Append(stack, unit);
    }
}

void Units_Move(const Units units)
{
    StackUnits(units); // XXX. Save for doing at end.
}
