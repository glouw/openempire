#include "Units.h"

#include "Util.h"
#include "File.h"
#include "Field.h"
#include "Rect.h"
#include "Surface.h"
#include "Tiles.h"
#include "Graphics.h"

#include <stdlib.h>

static Units GenerateTestZone(Units units, const Grid grid)
{
    //for(int32_t i = 0; i < 100; i++)
    for(int32_t i = 0; i < 10; i++)
    {
        const Point cart = {
            Util_Rand() % units.cols,
            Util_Rand() % units.rows,
        };
        //switch(Util_Rand() % 5)
        switch(0)
        {
        default:
        case 0:
            units = Units_Append(units, Unit_Make(cart, grid, FILE_MALE_VILLAGER_STANDING)); // XXX. GIMME SPEED.
            break;
        case 1:
            units = Units_Append(units, Unit_Make(cart, grid, FILE_FOREST_TREE));
            units = Units_Append(units, Unit_Make(cart, grid, FILE_FOREST_TREE_SHADOW));
            break;
        case 2:
            units = Units_Append(units, Unit_Make(cart, grid, FILE_GOLD_MINE));
            break;
        case 3:
            units = Units_Append(units, Unit_Make(cart, grid, FILE_STONE_MINE));
            break;
        case 4:
            units = Units_Append(units, Unit_Make(cart, grid, FILE_BERRY_BUSH));
            break;
        }
    }
    return units;
}

Units Units_New(const int32_t max, const Grid grid)
{
    const int32_t area = grid.rows * grid.cols;
    Unit* const unit = UTIL_ALLOC(Unit, max);
    Stack* const stack = UTIL_ALLOC(Stack, area);
    UTIL_CHECK(unit);
    UTIL_CHECK(stack);
    for(int32_t i = 0; i < area; i++)
        stack[i] = Stack_Build(8);
    Units units = { unit, 0, max, stack, grid.rows, grid.cols };
    units = GenerateTestZone(units, grid);
    return units;
}

Units Units_Append(Units units, Unit unit)
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

static void Select(const Units units, const Overview overview, const Input input, const Registrar graphics)
{
    const Quad quad = Overview_GetRenderBox(overview, -200); // XXX, Border needs to be equal to largest building size.
    const Points points = Quad_GetRenderPoints(quad);
    const Tiles tiles = Tiles_PrepGraphics(graphics, overview, units, points); // XXX. A little excessive, as this is done in the renderer, but its gets the job done.
    const Point click = { input.x, input.y };
    if(input.lu)
    {
        UnSelectAll(units);
        if(Overview_IsSelectionBoxBigEnough(overview))
            Tiles_SelectWithBox(tiles, overview.selection_box);
        else
        {
            const Tile tile = Tiles_SelectOne(tiles, click);
            if(tile.reference)
                if(input.key[SDL_SCANCODE_LCTRL])
                    Tiles_SelectSimilar(tiles, tile);
        }
    }
    Points_Free(points);
    Tiles_Free(tiles);
}

static void ApplyPathsToSelected(const Units units, const Map map, const Point cart_goal, const Point cart_grid_offset_goal)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->selected)
        {
            if(unit->max_speed > 0) // No sense in calculating paths for units without speed scalars.
            {
                const Field field = Field_New(map, units); // XXX. Should really only construct this once, unless the map is always changing?
                Points_Free(unit->path);
                unit->path = Field_SearchBreadthFirst(field, unit->cart, cart_goal);
                unit->path_index = 0;
                unit->cart_grid_offset_goal = cart_grid_offset_goal;
                Field_Free(field);
                Points_Print(unit->path);
            }
        }
    }
}

static void Command(const Units units, const Overview overview, const Input input, const Map map)
{
    if(input.ru)
    {
        const Point click = { input.x, input.y };
        const Point cart_goal = Overview_IsoToCart(overview, click, false);
        const Point cart = Overview_IsoToCart(overview, click, true);
        const Point cart_grid_offset_goal = Grid_GetOffsetFromGridPoint(overview.grid, cart);
        if(Units_CanWalk(units, map, cart_goal))
            ApplyPathsToSelected(units, map, cart_goal, cart_grid_offset_goal);
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

static Point CoheseBoids(const Units units, const Unit unit)
{
    const Stack stack = Units_GetStackCart(units, unit.cart);
    const Point delta = Point_Sub(stack.center_of_mass, unit.cell);
    const Point cohesion = Point_Div(delta, 100); // XXX. What is a good divisor?
    static Point zero;
    return stack.count > 0 ? cohesion : zero;
}

static Point SeparateBoids(const Units units, const Unit unit)
{
    static Point zero;
    return zero;
}

static Point AlignBoids(const Units units, const Unit unit)
{
    static Point zero;
    return zero;
}

// See the boids pseudocode:
//   http://www.kfish.org/boids/pseudocode.html

static void Move(const Units units, const Grid grid)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        const Unit unit = units.unit[i];
        const Point a = CoheseBoids(units, unit);
        const Point b = SeparateBoids(units, unit);
        const Point c = AlignBoids(units, unit);
        const Point stressors = Point_Add(a, Point_Add(b, c));
        units.unit[i] = Unit_MoveAlongPath(units.unit[i], grid, stressors);
    }
}

static void SortStacks(const Units units)
{
    for(int32_t y = 0; y < units.rows; y++)
    for(int32_t x = 0; x < units.cols; x++)
    {
        const Point point = { x, y };
        const Stack stack = Units_GetStackCart(units, point);
        Stack_Sort(stack);
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

void Units_Caretake(const Units units, const Registrar graphics, const Overview overview, const Grid grid, const Input input, const Map map)
{
    Move(units, grid);
    ResetStacks(units);
    StackStacks(units);
    SortStacks(units);
    CalculateCenters(units);
    Select(units, overview, input, graphics);
    Command(units, overview, input, map);
}

bool Units_CanWalk(const Units units, const Map map, const Point point)
{
    const Terrain terrain = Map_GetTerrainFile(map, point);
    const Stack stack = Units_GetStackCart(units, point);
    return Terrain_IsWalkable(terrain)
        && Stack_IsWalkable(stack);
}
