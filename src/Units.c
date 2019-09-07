#include "Units.h"

#include "Util.h"
#include "File.h"
#include "Field.h"
#include "Rect.h"
#include "Surface.h"
#include "Tiles.h"
#include "Graphics.h"
#include "Config.h"

#include <stdlib.h>

Field Units_Field(const Units units, const Map map)
{
    static Field zero;
    Field field = zero;
    field.rows = map.rows;
    field.cols = map.cols;
    field.object = UTIL_ALLOC(char, field.rows * field.cols);
    for(int32_t row = 0; row < field.rows; row++)
    for(int32_t col = 0; col < field.cols; col++)
    {
        const Point point = { col, row };
        if(Units_CanWalk(units, map, point))
            Field_Set(field, point, FIELD_WALKABLE_SPACE);
        else
            Field_Set(field, point, FIELD_OBSTRUCT_SPACE);
    }
    return field;
}

static Units GenerateBattleZone(Units units, const Map map, const Grid grid, const Registrar graphics)
{
    const int32_t depth = 5;
    for(int32_t x = 0; x < depth; x++)
    for(int32_t y = 0; y < map.rows; y++)
    {
        const Point cart = { x, y };
        const Graphics file = y < map.rows / 3 ? FILE_KNIGHT_IDLE : FILE_TEUTONIC_KNIGHT_IDLE;
        units = Units_Append(units, Unit_Make(cart, grid, file, COLOR_BLU, graphics));
    }
    for(int32_t x = map.cols - depth; x < map.cols; x++)
    for(int32_t y = 0; y < map.rows; y++)
    {
        const Point cart = { x, y };
        units = Units_Append(units, Unit_Make(cart, grid, FILE_KNIGHT_IDLE, COLOR_RED, graphics));
    }
    const Point cart = { map.cols / 2, map.rows / 2 };
    units = Units_Append(units, Unit_Make(cart, grid, FILE_KNIGHT_IDLE, COLOR_BLU, graphics));
    const Field field = Units_Field(units, map);
    for(int32_t i = 0; i < units.count; i++)
    {
        static Point zero;
        Unit* const unit = &units.unit[i];
        if(unit->color == COLOR_BLU)
        {
            const Point point = { map.cols - 1, map.rows / 2 };
            Unit_FindPath(unit, point, zero, field);
        }
        else
        {
            const Point point = { 0, map.rows / 2 };
            Unit_FindPath(unit, point, zero, field);
        }
    }
    Field_Free(field);
    return units;
}

static Units GenerateVillieZone(Units units, const Grid grid, const Registrar graphics)
{
    const Point a = { 0, 0 };
    const Point b = { 0, 2 };
    const Point c = { 2, 0 };
    const Point d = { 2, 2 };
    const Point e = { 1, 1 };
    units = Units_Append(units, Unit_Make(a, grid, FILE_MALE_VILLAGER_IDLE, COLOR_BLU, graphics));
    units = Units_Append(units, Unit_Make(b, grid, FILE_MALE_VILLAGER_IDLE, COLOR_BLU, graphics));
    units = Units_Append(units, Unit_Make(c, grid, FILE_MALE_VILLAGER_IDLE, COLOR_BLU, graphics));
    units = Units_Append(units, Unit_Make(d, grid, FILE_MALE_VILLAGER_IDLE, COLOR_BLU, graphics));
    units = Units_Append(units, Unit_Make(e, grid, FILE_MALE_VILLAGER_IDLE, COLOR_RED, graphics));
    return units;
}

static Units GenerateBerryZone(Units units, const Grid grid, const Registrar graphics)
{
    const Point a = { grid.cols / 2 + 0, grid.cols / 2 - 1};
    const Point b = { grid.cols / 2 - 1, grid.cols / 2 + 1};
    const Point c = { grid.cols / 2 - 0, grid.cols / 2 + 1};
    const Point d = { grid.cols / 2 - 0, grid.cols / 2 - 2};
    const Point e = { grid.cols / 2 - 0, grid.cols / 2 - 4};
    const Point f = { grid.cols / 2 - 2, grid.cols / 2 - 4};
    units = Units_Append(units, Unit_Make(a, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics));
    units = Units_Append(units, Unit_Make(b, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics));
    units = Units_Append(units, Unit_Make(c, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics));
    units = Units_Append(units, Unit_Make(d, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics));
    units = Units_Append(units, Unit_Make(e, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics));
    units = Units_Append(units, Unit_Make(f, grid, FILE_BERRY_BUSH, COLOR_BLU, graphics));
    return units;
}

static Units GenerateRandomZone(Units units, const Grid grid, const Registrar graphics)
{
    const Point a = { grid.cols / 2 + 0, grid.cols / 2 - 1};
    const Point b = { grid.cols / 2 - 1, grid.cols / 2 + 1};
    units = Units_Append(units, Unit_Make(a, grid, FILE_RIGHT_CLICK_RED_ARROWS, COLOR_BLU, graphics));
    units = Units_Append(units, Unit_Make(b, grid, FILE_WAYPOINT_FLAG, COLOR_BLU, graphics));
    return units;
}

static Units GenerateTestZone(Units units, const Map map, const Grid grid, const Registrar graphics)
{
    switch(0)
    {
    default:
    case 0: return GenerateBattleZone(units, map, grid, graphics);
    case 1: return GenerateBerryZone(units, grid, graphics);
    case 2: return GenerateVillieZone(units, grid, graphics);
    case 3: return GenerateRandomZone(units, grid, graphics);
    }
}

Units Units_New(const int32_t max, const Map map, const Grid grid, const Registrar graphics)
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
    units = GenerateTestZone(units, map, grid, graphics);
    units.cpu_count = 1 * SDL_GetCPUCount();
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
    return &units.stack[p.x + p.y * units.cols];
}

// Returns a shallow copy of the stack to prevent write access.
Stack Units_GetStackCart(const Units units, const Point p)
{
    static Stack zero;
    return OutOfBounds(units, p) ? zero : *GetStack(units, p);
}

static Units UnSelectAll(Units units)
{
    units.select_count = 0;
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].is_selected = false;
    return units;
}

static Units Select(Units units, const Overview overview, const Input input, const Registrar graphics, const Points render_points)
{
    const Tiles tiles = Tiles_PrepGraphics(graphics, overview, units, render_points);
    if(input.lu)
    {
        units = UnSelectAll(units);
        if(Overview_IsSelectionBoxBigEnough(overview))
            units.select_count = Tiles_SelectWithBox(tiles, overview.selection_box);
        else
        {
            const Tile tile = Tiles_SelectOne(tiles, input.point);
            if(tile.reference
            && !State_IsDead(tile.reference->state)
            && input.key[SDL_SCANCODE_LCTRL])
                units.select_count = Tiles_SelectSimilar(tiles, tile);
            else
                units.select_count = 1;
        }
    }
    Tiles_Free(tiles);
    return units;
}

static void FindPathForSelected(const Units units, const Point cart_goal, const Point cart_grid_offset_goal, const Field field)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->is_selected
        && unit->max_speed > 0)
        {
            unit->command_group = units.command_group_next;
            unit->command_group_count = units.select_count;
            Unit_FindPath(unit, cart_goal, cart_grid_offset_goal, field);
        }
    }
}

static Units Command(Units units, const Overview overview, const Input input, const Registrar graphics, const Map map, const Field field)
{
    if(input.ru)
    {
        const Point cart_goal = Overview_IsoToCart(overview, input.point, false);
        const Point cart = Overview_IsoToCart(overview, input.point, true);
        const Point cart_grid_offset_goal = Grid_GetOffsetFromGridPoint(overview.grid, cart);
        if(Units_CanWalk(units, map, cart_goal))
        {
            units.command_group_next++;
            FindPathForSelected(units, cart_goal, cart_grid_offset_goal, field);

            units = Units_Append(units, Unit_Make(cart_goal, overview.grid, FILE_RIGHT_CLICK_RED_ARROWS, COLOR_BLU, graphics));
        }
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

static Point CoheseBoids(const Units units, Unit* const unit)
{
    static Point zero;
    if(!State_IsDead(unit->state))
    {
        const Stack stack = Units_GetStackCart(units, unit->cart);
        const Point delta = Point_Sub(stack.center_of_mass, unit->cell);
        return Point_Div(delta, CONFIG_UNITS_COHESE_DIVISOR);
    }
    return zero;
}

static Point Nudge(void)
{
    const Point nudge = {
        1000 * ((Util_Rand() % 1000) - 500),
        1000 * ((Util_Rand() % 1000) - 500),
    };
    return nudge;
}

static Point Separate(Unit* const unit, Unit* const other)
{
    static Point zero;
    if(!State_IsDead(other->state) && unit->id != other->id)
    {
        const Point diff = Point_Sub(other->cell, unit->cell);
        if(Point_IsZero(diff))
            return Nudge();
        const int32_t width = UTIL_MAX(unit->width, other->width);
        if(Point_Mag(diff) < width)
            return Point_Sub(Point_Normalize(diff, width), diff);
    }
    return zero;
}

static Point SeparateBoids(const Units units, Unit* const unit)
{
    const int32_t width = 1;
    static Point zero;
    Point out = zero;
    if(!State_IsDead(unit->state))
    {
        for(int32_t x = -width; x <= width; x++)
        for(int32_t y = -width; y <= width; y++)
        {
            const Point cart_offset = { x, y };
            const Point cart = Point_Add(unit->cart, cart_offset);
            const Stack stack = Units_GetStackCart(units, cart);
            for(int32_t i = 0; i < stack.count; i++)
            {
                Unit* const other = stack.reference[i];
                const Point force = Separate(unit, other);
                out = Point_Sub(out, force);
            }
        }
    }
    return Point_Div(out, CONFIG_UNITS_SEPARATION_DIVISOR);
}

static Point AlignBoids(const Units units, Unit* const unit)
{
    const int32_t width = 1;
    static Point zero;
    Point out = zero;
    if(!State_IsDead(unit->state))
    {
        for(int32_t x = -width; x <= width; x++)
        for(int32_t y = -width; y <= width; y++)
        {
            const Point cart_offset = { x, y };
            const Point cart = Point_Add(unit->cart, cart_offset);
            const Stack stack = Units_GetStackCart(units, cart);
            for(int32_t i = 0; i < stack.count; i++)
            {
                Unit* const other = stack.reference[i];
                if(!State_IsDead(other->state)
                && unit->id != other->id
                && Unit_InPlatoon(unit, other))
                    out = Point_Add(out, other->velocity);
            }
        }
        return Point_Div(out, CONFIG_UNITS_ALIGN_DIVISOR);
    }
    return zero;
}

static Point WallPushBoids(const Units units, Unit* const unit, const Map map, const Grid grid)
{
    static Point zero;
    Point out = zero;
    if(!State_IsDead(unit->state))
    {
        const Point n = {  0, -1 };
        const Point e = { +1,  0 };
        const Point s = {  0, +1 };
        const Point w = { -1,  0 };
        const bool can_walk_n = Units_CanWalk(units, map, Point_Add(unit->cart, n));
        const bool can_walk_e = Units_CanWalk(units, map, Point_Add(unit->cart, e));
        const bool can_walk_s = Units_CanWalk(units, map, Point_Add(unit->cart, s));
        const bool can_walk_w = Units_CanWalk(units, map, Point_Add(unit->cart, w));
        const Point offset = Grid_GetCornerOffset(grid, unit->cart_grid_offset);
        const int32_t repulsion = 10 * unit->accel; // XXX. How strong should this be?
        const int32_t border = 10;
        if(!can_walk_n && offset.y < border) out = Point_Add(out, Point_Mul(s, repulsion));
        if(!can_walk_w && offset.x < border) out = Point_Add(out, Point_Mul(e, repulsion));
        if(!can_walk_s && offset.y > grid.tile_cart_height - border) out = Point_Add(out, Point_Mul(n, repulsion));
        if(!can_walk_e && offset.x > grid.tile_cart_width  - border) out = Point_Add(out, Point_Mul(w, repulsion));
    }
    return out;
}

static void CalculateBoidStressors(const Units units, Unit* const unit, const Map map, const Grid grid)
{
    static Point zero;
    if(!State_IsDead(unit->state))
    {
        unit->group_alignment = AlignBoids(units, unit);
        const Point cohese = unit->command_group_count > CONFIG_UNIT_COHESION_COUNT ? CoheseBoids(units, unit) : zero;
        const Point point[] = {
            unit->group_alignment,
            cohese,
            SeparateBoids(units, unit),
            WallPushBoids(units, unit, map, grid),
        };
        Point stressors = zero;
        for(int32_t j = 0; j < UTIL_LEN(point); j++)
            stressors = Point_Add(stressors, point[j]);
        unit->stressors = Point_Mag(stressors) < CONFIG_UNITS_STRESSOR_DEADZONE ? zero : stressors;
    }
}

static void ConditionallyStopBoids(const Units units, Unit* const unit)
{
    if(!State_IsDead(unit->state))
    {
        const Stack stack = Units_GetStackCart(units, unit->cart);
        for(int32_t i = 0; i < stack.count; i++)
        {
            Unit* const other = stack.reference[i];
            if(!State_IsDead(other->state)
            && unit->id != other->id
            && unit->path.count == 0
            && Unit_InPlatoon(unit, other))
                Unit_FreePath(other);
        }
    }
}

static void Kill(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(!State_IsDead(unit->state))
            if(unit->health <= 0)
                Unit_Kill(unit);
    }
}

static void Expire(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->can_expire
        && unit->state_timer == Unit_GetLastExpireTick(unit))
            unit->is_fully_decayed = true;
    }
}

static Unit* GetClosestBoid(const Units units, Unit* const unit)
{
    const int32_t width = 1;
    Unit* closest = NULL;
    int32_t max = INT32_MAX;
    for(int32_t x = -width; x <= width; x++)
    for(int32_t y = -width; y <= width; y++)
    {
        const Point cart_offset = { x, y };
        const Point cart = Point_Add(unit->cart, cart_offset);
        const Stack stack = Units_GetStackCart(units, cart);
        for(int32_t i = 0; i < stack.count; i++)
        {
            Unit* const other = stack.reference[i];
            if(other->color != unit->color
            && !State_IsDead(other->state))
            {
                const Point diff = Point_Sub(other->cell, unit->cell);
                const int32_t mag = Point_Mag(diff);
                if(mag < max)
                {
                    max = mag;
                    closest = other;
                }
            }
        }
    }
    return closest;
}

static void ChaseBoids(const Units units, Unit* const unit)
{
    if(!State_IsDead(unit->state))
    {
        Unit* const closest = GetClosestBoid(units, unit);
        if(closest != NULL)
        {
            unit->is_chasing = true;
            unit->interest = closest;
            Unit_MockPath(unit, closest->cart, closest->cart_grid_offset);
        }
        else
        {
            unit->is_chasing = false;
            unit->interest = NULL;
        }
    }
}

static Units Repath(Units units, const Field field)
{
    int32_t slice = units.count / CONFIG_UNITS_REPATH_SLICE_SIZE;
    if(slice == 0)
        slice = units.count;
    int32_t end = slice + units.repath_index;
    if(end > units.count)
        end = units.count;
    for(int32_t i = units.repath_index; i < end; i++)
        Unit_Repath(&units.unit[i], field);
    units.repath_index += slice;
    if(units.repath_index >= units.count)
        units.repath_index = 0;
    return units;
}

static Units RunHardRules(Units units, const Field field)
{
    units = Repath(units, field);
    for(int32_t i = 0; i < units.count; i++)
        ConditionallyStopBoids(units, &units.unit[i]);
    for(int32_t i = 0; i < units.count; i++)
        ChaseBoids(units, &units.unit[i]);
    for(int32_t i = 0; i < units.count; i++)
        Unit_Melee(&units.unit[i]);
    return units;
}

typedef struct
{
    Units units;
    Map map;
    Grid grid;
    int32_t a;
    int32_t b;
}
Needle;

static int32_t RunStressorNeedle(void* data)
{
    Needle* const needle = (Needle*) data;
    for(int32_t i = needle->a; i < needle->b; i++)
    {
        Unit* const unit = &needle->units.unit[i];
        CalculateBoidStressors(needle->units, unit, needle->map, needle->grid);
    }
    return 0;
}

static void BulkProcess(const Units units, const Map map, const Grid grid, int32_t Run(void* data))
{
    Needle* const needles = UTIL_ALLOC(Needle, units.cpu_count);
    SDL_Thread** const threads = UTIL_ALLOC(SDL_Thread*, units.cpu_count);
    UTIL_CHECK(needles);
    UTIL_CHECK(threads);
    const int32_t width = units.count / units.cpu_count;
    const int32_t remainder = units.count % units.cpu_count;
    for(int32_t i = 0; i < units.cpu_count; i++)
    {
        needles[i].units = units;
        needles[i].map = map;
        needles[i].grid = grid;
        needles[i].a = (i + 0) * width;
        needles[i].b = (i + 1) * width;
    }
    needles[units.cpu_count - 1].b += remainder;
    for(int32_t i = 0; i < units.cpu_count; i++) threads[i] = SDL_CreateThread(Run, "N/A", &needles[i]);
    for(int32_t i = 0; i < units.cpu_count; i++) SDL_WaitThread(threads[i], NULL);
    free(needles);
    free(threads);
}

static int32_t RunFlowNeedle(void* data)
{
    Needle* const needle = (Needle*) data;
    for(int32_t i = needle->a; i < needle->b; i++)
    {
        Unit* const unit = &needle->units.unit[i];
        if(!State_IsDead(unit->state))
        {
            Unit_Flow(unit, needle->grid);
            Unit_Move(unit, needle->grid);
            if(!Units_CanWalk(needle->units, needle->map, unit->cart))
                Unit_UndoMove(unit, needle->grid);
        }
    }
    return 0;
}

// See the boids pseudocode: http://www.kfish.org/boids/pseudocode.html
static Units ManagePathFinding(const Units units, const Grid grid, const Map map, const Field field)
{
    BulkProcess(units, map, grid, RunStressorNeedle);
    BulkProcess(units, map, grid, RunFlowNeedle);
    return RunHardRules(units, field);
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

void Delete(const Units units, const Input input)
{
    if(input.key[SDL_SCANCODE_DELETE])
        for(int32_t i = 0; i < units.count; i++)
        {
            Unit* const unit = &units.unit[i];
            if(unit->is_selected && !State_IsDead(unit->state))
                Unit_Kill(unit);
        }
}

static void Tick(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        unit->state_timer++;
        unit->dir_timer++;
        unit->path_index_timer++;
    }
}

static void Decay(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        const int32_t last_tick = Unit_GetLastFallTick(unit);
        if(unit->state == STATE_FALL
        && unit->state_timer == last_tick)
        {
            Unit_SetState(unit, STATE_DECAY, true, true);
            unit->is_selected = false;
        }
    }
}

static void ManageStacks(const Units units)
{
    ResetStacks(units);
    StackStacks(units);
    SortStacks(units);
    CalculateCenters(units);
}

static int32_t CompareByFullyDecayed(const void* a, const void* b)
{
    Unit* const aa = (Unit*) a;
    Unit* const bb = (Unit*) b;
    return aa->is_fully_decayed > bb->is_fully_decayed;
}

static void SortByFullyDecayed(const Units units)
{
    qsort(units.unit, units.count, sizeof(*units.unit), CompareByFullyDecayed);
}

static void FlagDecayed(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        const int32_t last_tick = Unit_GetLastDecayTick(unit);
        if(unit->state == STATE_DECAY
        && unit->state_timer == last_tick)
            unit->is_fully_decayed = true;
    }
}

static Units ResizeDecayed(Units units)
{
    int32_t i = 0;
    for(i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->is_fully_decayed)
        {
            units.count = i;
            break;
        }
    }
    return units;
}

static Units RemoveTheDecayed(const Units units)
{
    FlagDecayed(units);
    SortByFullyDecayed(units);
    return ResizeDecayed(units);
}

static Units ManageAction(Units units, const Registrar graphics, const Overview overview, const Input input, const Map map, const Field field, const Points render_points)
{
    units = Select(units, overview, input, graphics, render_points);
    units = Command(units, overview, input, graphics, map, field);
    Tick(units);
    Decay(units);
    Delete(units, input);
    Kill(units);
    Expire(units);
    return units;
}

// Stacks need to be updated after the pathfinder runs, else the video renderer will use stale stack data and create tile jumping artifacts.
Units Units_Caretake(Units units, const Registrar graphics, const Overview overview, const Grid grid, const Input input, const Map map, const Field field, const Points render_points)
{
    units = ManagePathFinding(units, grid, map, field);
    units = RemoveTheDecayed(units);
    ManageStacks(units);
    units = ManageAction(units, graphics, overview, input, map, field, render_points);
    return units;
}

bool Units_CanWalk(const Units units, const Map map, const Point point)
{
    const Terrain terrain = Map_GetTerrainFile(map, point);
    const Stack stack = Units_GetStackCart(units, point);
    return stack.reference != NULL
        && Terrain_IsWalkable(terrain)
        && Stack_IsWalkable(stack);
}
