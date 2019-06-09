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
    for(int32_t i = 0; i < 500; i++)
    {
        const Point cart = {
            Util_Rand() % units.cols,
            Util_Rand() % units.rows,
        };
        switch(Util_Rand() % 5)
        {
        default:
        case 0:
            units = Units_Append(units, Unit_Make(cart, grid, FILE_MALE_VILLAGER_STANDING)); // XXX. GIMME SPEED.
            break;
#if 0
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
#endif
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
    static Units zero;
    Units units = zero;
    units.unit = unit;
    units.max = max;
    units.stack = stack;
    units.rows = grid.rows;
    units.cols = grid.cols;
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
    unit.id = units.id_next++;
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

static Units UnSelectAll(Units units)
{
    units.select_count = 0;
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].selected = false;
    return units;
}

static Units Select(Units units, const Overview overview, const Input input, const Registrar graphics)
{
    const Quad quad = Overview_GetRenderBox(overview, -200); // XXX, Border needs to be equal to largest building size.
    const Points points = Quad_GetRenderPoints(quad);
    const Tiles tiles = Tiles_PrepGraphics(graphics, overview, units, points); // XXX. A little excessive, as this is done in the renderer, but its gets the job done.
    if(input.lu)
    {
        units = UnSelectAll(units);
        if(Overview_IsSelectionBoxBigEnough(overview))
            units.select_count = Tiles_SelectWithBox(tiles, overview.selection_box);
        else
        {
            const Tile tile = Tiles_SelectOne(tiles, input.point);
            if(tile.reference
            && input.key[SDL_SCANCODE_LCTRL])
                units.select_count = Tiles_SelectSimilar(tiles, tile);
            else
                units.select_count = 1;
        }
    }
    Points_Free(points);
    Tiles_Free(tiles);
    return units;
}

static Unit ApplyPath(const Units units, Unit unit, const Map map, const Point cart_goal, const Point cart_grid_offset_goal)
{
    const Field field = Field_New(map, units); // XXX. Should really only construct this once, unless the map is always changing?
    Points_Free(unit.path);
    unit.path = Field_SearchBreadthFirst(field, unit.cart, cart_goal);
    unit.path_index = 0;
    unit.cart_grid_offset_goal = cart_grid_offset_goal;
    unit.command_group = units.command_group_next;
    Field_Free(field);
    return unit;
}

static void ApplyPathToSelected(const Units units, const Map map, const Point cart_goal, const Point cart_grid_offset_goal)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        const Unit unit = units.unit[i];
        if(unit.selected
        && unit.max_speed > 0)
            units.unit[i] = ApplyPath(units, unit, map, cart_goal, cart_grid_offset_goal);
    }
}

static Units Command(Units units, const Overview overview, const Input input, const Map map)
{
    if(input.ru)
    {
        units.command_group_next += 1;
        const Point cart_goal = Overview_IsoToCart(overview, input.point, false);
        const Point cart = Overview_IsoToCart(overview, input.point, true);
        const Point cart_grid_offset_goal = Grid_GetOffsetFromGridPoint(overview.grid, cart);
        if(Units_CanWalk(units, map, cart_goal))
            ApplyPathToSelected(units, map, cart_goal, cart_grid_offset_goal);
    }
    return units;
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
    static Point zero;
    const Stack stack = Units_GetStackCart(units, unit.cart);
    const Point delta = Point_Sub(stack.center_of_mass, unit.cell);
    const Point cohesion = Point_Div(delta, 128); // XXX. What is a good divisor?
    return stack.count > 0 ? cohesion : zero;
}

static Point SeparateBoids(const Units units, const Unit unit)
{
    static Point zero;
    Point out = zero;
    for(int32_t x = -1; x <= 1; x++)
    for(int32_t y = -1; y <= 1; y++)
    {
        const Point offset = { x, y };
        const Point cart = Point_Add(unit.cart, offset);
        const Stack stack = Units_GetStackCart(units, cart);
        for(int32_t i = 0; i < stack.count; i++)
        {
            Unit* const other = stack.reference[i];
            if(unit.id != other->id)
            {
                const Point diff = Point_Sub(other->cell, unit.cell);
                if(Point_Mag(diff) < 15000) // XXX. What is a good width?
                    out = Point_Sub(out, diff);
            }
        }
    }
    return Point_Div(out, 32);
}

static Point AlignBoids(const Units units, const Unit unit)
{
    static Point zero;
    Point out = zero;
    const Stack stack = Units_GetStackCart(units, unit.cart);
    if(stack.count > 1)
    {
        for(int32_t i = 0; i < stack.count; i++)
        {
            Unit* const other = stack.reference[i];
            if(unit.id != other->id)
                out = Point_Add(out, other->velocity);
        }
        out = Point_Div(out, stack.count - 1);
        return Point_Div(Point_Sub(out, unit.velocity), 8);
    }
    return zero;
}

// Boids, when swept up in a current of other boids, will
// try to go back to a path point if they were swept past the path point.
// This function ensures all boids on a tile share the same path index
// so the group acts like it guided by a single leader.

static void UnifyBoids(const Units units, const Unit unit)
{
    const Stack stack = Units_GetStackCart(units, unit.cart);
    const int32_t max = Stack_GetMaxPathIndex(stack);
    for(int32_t i = 0; i < stack.count; i++)
    {
        Unit* const other = stack.reference[i];
        if(max < other->path.count
        && unit.command_group == other->command_group)
            other->path_index = max;
    }
}

// Boids, when reaching their final destination, will struggle in
// a "mosh pit" like style to reach the final point in the grid tile.
// A simple solution is to stop all boids from reaching their final point
// within a grid tile by stopping all boids within the grid tile.

static void StopBoids(const Units units, const Unit unit)
{
    const Stack stack = Units_GetStackCart(units, unit.cart);
    for(int32_t i = 0; i < stack.count; i++)
    {
        Unit* const other = stack.reference[i];
        if(unit.path.count == 0
        && unit.command_group == other->command_group)
            other->path = Points_Free(other->path);
    }
}

// See the boids pseudocode:
//   http://www.kfish.org/boids/pseudocode.html

static void PathFindBoid(const Units units, const Grid grid, const Map map)
{
    for(int32_t i = 0; i < units.count; i++) // XXX. Cannot be threaded due to read writes.
    {
        Unit unit = units.unit[i];

        // Logic control rules.

        UnifyBoids(units, unit);
        StopBoids(units, unit);

        // Stressor rules.

        const Point point[] = {
            CoheseBoids(units, unit),
            SeparateBoids(units, unit),
            AlignBoids(units, unit),
            // XXX. Need a separation rule for immoveable objects.
        };
        static Point zero;
        Point stressors = zero;
        for(int32_t j = 0; j < UTIL_LEN(point); j++)
            stressors = Point_Add(stressors, point[j]);

        // Follow path and apply stressors.

        unit = Unit_FollowPath(unit, grid);
        unit.velocity = Point_Add(unit.velocity, stressors);
        unit = Unit_CapSpeed(unit);
        if(Units_CanWalk(units, map, unit.cart))
            unit = Unit_Move(unit, grid);
        units.unit[i] = unit;
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

Units Units_Caretake(Units units, const Registrar graphics, const Overview overview, const Grid grid, const Input input, const Map map)
{
    PathFindBoid(units, grid, map);
    ResetStacks(units);
    StackStacks(units);
    SortStacks(units);
    CalculateCenters(units);
    units = Select(units, overview, input, graphics);
    units = Command(units, overview, input, map);
    return units;
}

bool Units_CanWalk(const Units units, const Map map, const Point point)
{
    const Terrain terrain = Map_GetTerrainFile(map, point);
    const Stack stack = Units_GetStackCart(units, point);
    return stack.reference != NULL // Out of bounds check.
        && Terrain_IsWalkable(terrain)
        && Stack_IsWalkable(stack);
}
