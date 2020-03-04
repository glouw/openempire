#include "Units.h"

#include "Window.h"
#include "Util.h"
#include "File.h"
#include "Field.h"
#include "Icon.h"
#include "Rect.h"
#include "Surface.h"
#include "Tiles.h"
#include "Graphics.h"
#include "Config.h"

#include <stdlib.h>

static bool CanWalk(const Units units, const Map map, const Point point)
{
    const Terrain terrain = Map_GetTerrainFile(map, point);
    const Stack stack = Units_GetStackCart(units, point);
    return stack.reference != NULL
        && Terrain_IsWalkable(terrain)
        && Stack_IsWalkable(stack);
}

static bool CanBuild(const Units units, const Map map, Unit* const unit)
{
    if(!unit->trait.can_expire)
        for(int32_t y = 0; y < unit->trait.dimensions.y; y++)
        for(int32_t x = 0; x < unit->trait.dimensions.x; x++)
        {
            const Point offset = { x, y };
            const Point cart = Point_Add(unit->cart, offset);
            if(!CanWalk(units, map, cart))
                return false;
        }
    return true;
}

Field Units_Field(const Units units, const Map map)
{
    static Field zero;
    Field field = zero;
    field.size = map.size;
    field.object = UTIL_ALLOC(char, field.size * field.size);
    for(int32_t row = 0; row < field.size; row++)
    for(int32_t col = 0; col < field.size; col++)
    {
        const Point point = { col, row };
        CanWalk(units, map, point)
            ? Field_Set(field, point, FIELD_WALKABLE_SPACE)
            : Field_Set(field, point, FIELD_OBSTRUCT_SPACE);
    }
    return field;
}

static Units Alloc(Units units)
{
    const int32_t area = units.size * units.size;
    units.unit = UTIL_ALLOC(Unit, units.max);
    units.stack = UTIL_ALLOC(Stack, area);
    for(int32_t i = 0; i < area; i++)
        units.stack[i] = Stack_Build(8);
    return units;
}

Units Units_New(const int32_t size, const int32_t cpu_count, const int32_t max, const Color color, const Civ civ)
{
    static Units zero;
    Units units = zero;
    units.size = size;
    units.max = max;
    units.cpu_count = cpu_count;
    units.stamp[color].status.age = AGE_1;
    units.stamp[color].status.civ = civ;
    units.stamp[color].motive.action = ACTION_NONE;
    units.stamp[color].motive.type = TYPE_NONE;
    units.color = color;
    return Alloc(units);
}

void Units_Free(const Units units)
{
    const int32_t area = units.size * units.size;
    for(int32_t i = 0; i < area; i++)
        Stack_Free(units.stack[i]);
    free(units.stack);
    free(units.unit);
}

static Units UnSelectAll(Units units)
{
    units.select_count = 0;
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].is_selected = false;
    return units;
}

static Units RecountSelected(Units units)
{
    units.select_count = 0;
    for(int32_t i = 0; i < units.count; i++)
        if(units.unit[i].is_selected)
            units.select_count++;
    return units;
}

static Units Select(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Points render_points)
{
    if(overview.event.mouse_lu && !overview.event.key_left_shift)
    {
        const Tiles tiles = Tiles_PrepGraphics(graphics, overview, grid, units, render_points);
        Tiles_SortByHeight(tiles); // FOR SELECTING TRANSPARENT UNITS BEHIND INANIMATES OR TREES.
        units = UnSelectAll(units);
        if(Overview_IsSelectionBoxBigEnough(overview))
            units.select_count = Tiles_SelectWithBox(tiles, overview.selection_box);
        else
        {
            const Tile tile = Tiles_Get(tiles, overview.mouse_cursor);
            if(tile.reference)
            {
                Tile_Select(tile);
                units.select_count = overview.event.key_left_ctrl ? Tiles_SelectSimilar(tiles, tile) : 1;
            }
        }
        Tiles_Free(tiles);
    }
    return units;
}

static void FindPathForSelected(const Units units, const Overview overview, const Point cart_goal, const Point cart_grid_offset_goal, const Field field)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->color == overview.color && unit->is_selected && unit->trait.max_speed > 0)
        {
            unit->command_group = units.command_group_next;
            unit->command_group_count = units.select_count;
            Unit_FindPath(unit, cart_goal, cart_grid_offset_goal, field);
        }
    }
}

static Units Append(Units units, const Unit unit)
{
    if(units.count == units.max)
        Util_Bomb("OUT OF MEMORY\n");
    units.unit[units.count++] = unit;
    return units;
}

static Units BulkAppend(Units units, const Map map, Unit unit[], const int32_t len, const bool ignore_collisions)
{
    if(!ignore_collisions)
        for(int32_t i = 0; i < len; i++)
            if(!CanBuild(units, map, &unit[i]))
                return units;
    for(int32_t i = 0; i < len; i++)
        units = Append(units, unit[i]);
    return units;
}

static void SetChildren(Unit unit[], const int32_t count)
{
    if(count > 1)
    {
        unit[0].has_children = true;
        for(int32_t i = 1; i < count; i++)
            unit[i].parent_id = unit[0].id;
    }
}

static Units SpawnParts(Units units, const Point cart, const Point offset, const Grid grid, const Color color, const Registrar graphics, const Map map, const bool is_floating, const Parts parts, const bool ignore_collisions, const Trigger trigger)
{
    Unit* const temp = UTIL_ALLOC(Unit, parts.count);
    for(int32_t i = 0; i < parts.count; i++)
    {
        const Part part = parts.part[i];
        const Point cart_part = Point_Add(cart, part.cart);
        temp[i] = Unit_Make(cart_part, offset, grid, part.file, color, graphics, true, is_floating, trigger);
    }
    SetChildren(temp, parts.count);
    units = BulkAppend(units, map, temp, parts.count, ignore_collisions);
    free(temp);
    return units;
}

static void SetSelectedInterest(const Units units, Unit* const interest)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->is_selected)
            Unit_SetInterest(unit, interest);
    }
}

static void SetSelectedAttackMove(const Units units, const bool using_attack_move)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->is_selected)
            unit->using_attack_move = using_attack_move;
    }
}

static void DisengageSelected(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->is_selected)
            unit->is_engaged_in_melee = false;
    }
}

static Units Command(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map, const Field field, const Points render_points)
{
    const bool using_attack_move = Button_UseAttackMove(Button_FromOverview(overview));
    if(units.select_count > 0)
        if(overview.event.mouse_ru || using_attack_move)
        {
            const Tiles tiles = Tiles_PrepGraphics(graphics, overview, grid, units, render_points);
            const Tile tile = Tiles_Get(tiles, overview.mouse_cursor);
            if(tile.reference
            && tile.reference->color != overview.color)
            {
                DisengageSelected(units);
                SetSelectedInterest(units, tile.reference);
                FindPathForSelected(units, overview, tile.reference->cart, tile.reference->cart_grid_offset, field);
            }
            else
            {
                DisengageSelected(units);
                SetSelectedInterest(units, NULL);
                const Point cart_goal = Overview_IsoToCart(overview, grid, overview.mouse_cursor, false);
                const Point cart = Overview_IsoToCart(overview, grid, overview.mouse_cursor, true);
                const Point cart_grid_offset_goal = Grid_GetOffsetFromGridPoint(grid, cart);
                const Parts parts = Parts_GetRedArrows();
                FindPathForSelected(units, overview, cart_goal, cart_grid_offset_goal, field);
                units = SpawnParts(units, cart_goal, cart_grid_offset_goal, grid, COLOR_GAIA, graphics, map, false, parts, false, TRIGGER_NONE);
            }
            SetSelectedAttackMove(units, using_attack_move);
            Tiles_Free(tiles);
            units.command_group_next++;
        }
    return units;
}

static Point SeparateBoids(const Units units, Unit* const unit)
{
    const int32_t width = 1;
    static Point zero;
    Point out = zero;
    if(!Unit_IsExempt(unit))
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
                const Point force = Unit_Separate(unit, other);
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
    if(!Unit_IsExempt(unit))
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
                if(!Unit_IsExempt(other) && Unit_IsDifferent(unit, other) && Unit_InPlatoon(unit, other))
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
    if(!Unit_IsExempt(unit)) // XXX: HOW TO USE NORMAL VECTORS TO RUN ALONG WALLS?
    {
        const Point n = {  0, -1 };
        const Point e = { +1,  0 };
        const Point s = {  0, +1 };
        const Point w = { -1,  0 };
        const bool can_walk_n = CanWalk(units, map, Point_Add(unit->cart, n));
        const bool can_walk_e = CanWalk(units, map, Point_Add(unit->cart, e));
        const bool can_walk_s = CanWalk(units, map, Point_Add(unit->cart, s));
        const bool can_walk_w = CanWalk(units, map, Point_Add(unit->cart, w));
        const Point offset = Grid_GetCornerOffset(grid, unit->cart_grid_offset);
        const int32_t repulsion = 1000; // XXX: HOW STRONG SHOULD THIS BE?
        const int32_t border = 10;
        if(!can_walk_n && offset.y < border) out = Point_Add(out, Point_Mul(s, repulsion));
        if(!can_walk_w && offset.x < border) out = Point_Add(out, Point_Mul(e, repulsion));
        if(!can_walk_s && offset.y > grid.tile_cart_height - border) out = Point_Add(out, Point_Mul(n, repulsion));
        if(!can_walk_e && offset.x > grid.tile_cart_width  - border) out = Point_Add(out, Point_Mul(w, repulsion));
    }
    unit->was_wall_pushed = Point_Mag(out) > 0;
    return out;
}

static void CalculateBoidStressors(const Units units, Unit* const unit, const Map map, const Grid grid)
{
    static Point zero;
    if(!Unit_IsExempt(unit))
    {
        unit->group_alignment = AlignBoids(units, unit);
        const Point point[] = {
            unit->group_alignment,
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
    if(!Unit_IsExempt(unit))
    {
        const Stack stack = Units_GetStackCart(units, unit->cart);
        for(int32_t i = 0; i < stack.count; i++)
        {
            Unit* const other = stack.reference[i];
            if(!Unit_IsExempt(other) && Unit_IsDifferent(unit, other) && Unit_HasNoPath(unit) && Unit_InPlatoon(unit, other))
                Unit_FreePath(other);
        }
    }
}

void Units_FreeAllPathsForRecovery(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        // PATHS ARE NOT TRANSFERRED OVER THE WIRE.
        // SINCE UNITS ARE TRANSFERRED TO OTHER CLIENTS
        // THEY MUST REPATH ON SAID CLIENTS ONCE RECOVERED.
        if(Unit_HasPath(unit))
            unit->must_repath_with_recover = true;
        Unit_FreePath(unit);
    }
}

static void ConditionallyStopAllBoids(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
        ConditionallyStopBoids(units, &units.unit[i]);
}

static bool EqualDimension(Point dimensions, const Graphics file)
{
    const int32_t min = UTIL_MIN(dimensions.x, dimensions.y);
    dimensions.x = min;
    dimensions.y = min;
    const Point _1x1_ = FILE_DIMENSIONS_1X1;
    const Point _2x2_ = FILE_DIMENSIONS_2X2;
    const Point _3x3_ = FILE_DIMENSIONS_3X3;
    const Point _4x4_ = FILE_DIMENSIONS_4X4;
    const Point _5x5_ = FILE_DIMENSIONS_5X5;
    switch(file)
    {
        default:
        case FILE_GRAPHICS_RUBBLE_1X1: return Point_Equal(_1x1_, dimensions);
        case FILE_GRAPHICS_RUBBLE_2X2: return Point_Equal(_2x2_, dimensions);
        case FILE_GRAPHICS_RUBBLE_3X3: return Point_Equal(_3x3_, dimensions);
        case FILE_GRAPHICS_RUBBLE_4X4: return Point_Equal(_4x4_, dimensions);
        case FILE_GRAPHICS_RUBBLE_5X5: return Point_Equal(_5x5_, dimensions);
    }
}

static Units SpamFire(Units units, Unit* const unit, const Grid grid, const Registrar graphics, const Map map)
{
    for(int32_t x = 0; x < unit->trait.dimensions.x; x++)
    for(int32_t y = 0; y < unit->trait.dimensions.y; y++)
    {
        const Point offset = { x, y };
        const Point cart = Point_Add(unit->cart, offset);
        const int32_t w = grid.tile_cart_width;
        const int32_t h = grid.tile_cart_height;
        const Point grid_offset = {
            Util_Rand() % w - w / 2,
            Util_Rand() % h - h / 2,
        };
        const Parts parts = Parts_GetFire();
        units = SpawnParts(units, cart, grid_offset, grid, COLOR_GAIA, graphics, map, false, parts, true, TRIGGER_NONE);
    }
    return units;
}

static Units SpamSmoke(Units units, Unit* const unit, const Grid grid, const Registrar graphics, const Map map)
{
    const Point zero = { 0,0 };
    for(int32_t x = 0; x < unit->trait.dimensions.x; x++)
    for(int32_t y = 0; y < unit->trait.dimensions.y; y++)
    {
        const Point shift = { x, y };
        const Point cart = Point_Add(unit->cart, shift);
        const Parts parts = Parts_GetSmoke();
        units = SpawnParts(units, cart, zero, grid, COLOR_GAIA, graphics, map, false, parts, true, TRIGGER_NONE);
    }
    return units;
}

void MakeRubble(Unit* unit, const Grid grid, const Registrar graphics)
{
    static Point none;
    const Graphics rubbles[] = {
        FILE_GRAPHICS_RUBBLE_1X1,
        FILE_GRAPHICS_RUBBLE_2X2,
        FILE_GRAPHICS_RUBBLE_3X3,
        FILE_GRAPHICS_RUBBLE_4X4,
        FILE_GRAPHICS_RUBBLE_5X5,
    };
    Graphics file = FILE_GRAPHICS_NONE;
    for(int32_t i = 0; i < UTIL_LEN(rubbles); i++)
    {
        const Graphics rubble = rubbles[i];
        if(EqualDimension(unit->trait.dimensions, rubble))
            file = rubble;
    }
    if(file != FILE_GRAPHICS_NONE)
        *unit = Unit_Make(unit->cart, none, grid, file, unit->color, graphics, false, false, TRIGGER_NONE);
}

static void KillChildren(const Units units, Unit* const unit)
{
    for(int32_t j = 0; j < units.count; j++)
    {
        Unit* const child = &units.unit[j];
        if(child->parent_id == unit->id)
            Unit_Kill(child);
    }
}

static void Kill(const Units units, Unit* const unit)
{
    Unit_Kill(unit);
    if(unit->has_children)
        KillChildren(units, unit);
}

static Units KillTheDead(Units units, const Grid grid, const Registrar graphics, const Map map)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(!Unit_IsExempt(unit) && Unit_IsDead(unit))
        {
            Kill(units, unit);
            if(unit->must_skip_debris)
                continue;
            if(unit->trait.is_inanimate)
            {
                MakeRubble(unit, grid, graphics);
                units = SpamFire(units, unit, grid, graphics, map);
                units = SpamSmoke(units, unit, grid, graphics, map);
            }
        }
    }
    return units;
}

static void Expire(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->trait.can_expire
        && unit->state_timer == Unit_GetLastExpireTick(unit))
            unit->must_garbage_collect = true;
    }
}

static Unit* GetClosestBoid(const Units units, Unit* const unit, const Grid grid)
{
    static Point zero;
    const int32_t width = 2;
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
            if(other->color != unit->color && !Unit_IsExempt(other)) // XXX: USE ALLY SYSTEM INSTEAD OF COLOR FREE FOR ALL.
            {
                Point cell = zero;
                if(other->trait.is_inanimate)
                {
                    cell = Grid_CartToCell(grid, cart);
                    const Point mid = {
                        CONFIG_GRID_CELL_SIZE / 2,
                        CONFIG_GRID_CELL_SIZE / 2,
                    };
                    cell = Point_Add(cell, mid);
                }
                else
                    cell = other->cell;
                const Point diff = Point_Sub(cell, unit->cell);
                const int32_t mag = Point_Mag(diff);
                if(mag < max)
                {
                    if(other->trait.is_inanimate)
                        other->cell_inanimate = cell;
                    max = mag;
                    closest = other;
                }
            }
        }
    }
    return closest;
}

static void EngageWithMock(Unit* const unit, Unit* const closest, const Grid grid)
{
    static Point zero;
    if(closest->trait.is_inanimate)
    {
        const Point cart = Grid_CellToCart(grid, closest->cell_inanimate);
        Unit_MockPath(unit, cart, zero);
    }
    else
        Unit_MockPath(unit, closest->cart, closest->cart_grid_offset);
    unit->is_engaged_in_melee = true;
    Unit_SetInterest(unit, closest);
}

static void EngageBoids(const Units units, Unit* const unit, const Grid grid)
{
    if(!Unit_IsExempt(unit))
    {
        Unit* const closest = GetClosestBoid(units, unit, grid);
        if(closest)
        {
            if(unit->using_attack_move)
                EngageWithMock(unit, closest, grid);
            else
            if(unit->interest == closest)
                EngageWithMock(unit, closest, grid);
        }
    }
}

static void EngageAllBoids(const Units units, const Grid grid)
{
    for(int32_t i = 0; i < units.count; i++)
        EngageBoids(units, &units.unit[i], grid);
}

static void DisengageAllBoids(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        unit->interest = NULL;
        unit->is_engaged_in_melee = false;
    }
}

// DOES NOT NEED TO BE THREADED -
// GIVES A MORE NATURAL FEEL TO HAVE THE PATHER REPATH IN TIME SLICES.
static Units RepathSomeBoids(Units units, const Field field)
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

static Status Gather(Status status, const Resource resource)
{
    switch(resource.type)
    {
        case TYPE_FOOD  : status.food  += resource.amount; break;
        case TYPE_WOOD  : status.wood  += resource.amount; break;
        case TYPE_GOLD  : status.gold  += resource.amount; break;
        case TYPE_STONE : status.stone += resource.amount; break;
        default         : break;
    }
    return status;
}

static Units MeleeAllBoids(Units units, const Grid grid)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        const Resource resource = Unit_Melee(unit, grid);
        if(resource.type != TYPE_NONE)
            units.stamp[unit->color].status = Gather(units.stamp[unit->color].status, resource);
    }
    return units;
}

static Units ProcessHardRules(Units units, const Field field, const Grid grid)
{
    units = RepathSomeBoids(units, field);
    ConditionallyStopAllBoids(units);
    EngageAllBoids(units, grid);
    return MeleeAllBoids(units, grid);
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

static int32_t StressorThread(void* data)
{
    Needle* const needle = (Needle*) data;
    for(int32_t i = needle->a; i < needle->b; i++)
    {
        Unit* const unit = &needle->units.unit[i];
        CalculateBoidStressors(needle->units, unit, needle->map, needle->grid);
    }
    return 0;
}

static void Process(const Units units, const Map map, const Grid grid, int32_t Run(void* data))
{
    Needle* const needles = UTIL_ALLOC(Needle, units.cpu_count);
    SDL_Thread** const threads = UTIL_ALLOC(SDL_Thread*, units.cpu_count);
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

static int32_t FlowThread(void* data)
{
    Needle* const needle = (Needle*) data;
    for(int32_t i = needle->a; i < needle->b; i++)
    {
        Unit* const unit = &needle->units.unit[i];
        if(!State_IsDead(unit->state))
        {
            Unit_Flow(unit, needle->grid);
            Unit_Move(unit, needle->grid);
            if(!CanWalk(needle->units, needle->map, unit->cart))
                Unit_UndoMove(unit, needle->grid);
        }
    }
    return 0;
}

static Units ManagePathFinding(const Units units, const Grid grid, const Map map, const Field field)
{
    Process(units, map, grid, StressorThread);
    Process(units, map, grid, FlowThread);
    return ProcessHardRules(units, field, grid);
}

static void Tick(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        unit->state_timer++;
        unit->dir_timer++;
        unit->path_index_timer++;
        if(unit->is_timing_to_collect)
            unit->garbage_collection_timer++;
    }
}

static void Decay(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        const int32_t last_tick = Unit_GetLastFallTick(unit);
        if(unit->state == STATE_FALL && unit->state_timer == last_tick)
        {
            Unit_SetState(unit, STATE_DECAY, true);
            unit->is_selected = false;
        }
    }
}

static void FlagGarbage(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        const int32_t last_tick = Unit_GetLastDecayTick(unit);
        if(unit->state == STATE_DECAY && unit->state_timer == last_tick)
            unit->must_garbage_collect = true;
        if(unit->is_timing_to_collect)
        {
            const int32_t time = (unit->trait.type == TYPE_FIRE)
                ? CONFIG_UNITS_CLEANUP_FIRE
                : CONFIG_UNITS_CLEANUP_RUBBLE;
            if(unit->garbage_collection_timer == time)
                unit->must_garbage_collect = true;
        }
    }
}

static Units Resize(Units units)
{
    for(int32_t index = 0; index < units.count; index++)
    {
        Unit* const unit = &units.unit[index];
        if(unit->must_garbage_collect)
        {
            units.count = index;
            break;
        }
    }
    return units;
}

static int32_t CompareByGarbage(const void* a, const void* b)
{
    Unit* const aa = (Unit*) a;
    Unit* const bb = (Unit*) b;
    return aa->must_garbage_collect > bb->must_garbage_collect;
}

static int32_t CompareById(const void* a, const void* b)
{
    Unit* const aa = (Unit*) a;
    Unit* const bb = (Unit*) b;
    return aa->id > bb->id;
}

static int32_t MatchInterestId(const void* a, const void* b)
{
    Unit* const aa = (Unit*) a;
    Unit* const bb = (Unit*) b;
    return (aa->interest_id == bb->id) == 0;
}

static void ResetInterests(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const a = &units.unit[i];
        Unit* const b = bsearch(a, units.unit, units.count, sizeof(*a), MatchInterestId);
        if(b)
            a->interest = b;
    }
}

static Units RemoveGarbage(Units units)
{
    FlagGarbage(units);
    UTIL_SORT(units.unit, units.count, CompareByGarbage);
    units = Resize(units);
    UTIL_SORT(units.unit, units.count, CompareById);
    ResetInterests(units);
    return units;
}

static void UpdateEntropy(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].entropy = Point_Rand();
}

static void Zero(int32_t array[], const int32_t size)
{
    for(int32_t i = 0; i < size; i++)
        array[i] = 0;
}

static int32_t MaxIndex(int32_t array[], const int32_t size)
{
    int32_t max = 0;
    int32_t index = 0;
    for(int32_t i = 0; i < size; i++)
        if(array[i] > max)
        {
            max = array[i];
            index = i;
        }
    return index;
}

static Action GetAction(const Units units, const Color color)
{
    int32_t counts[ACTION_COUNT + 1];
    const int32_t size = UTIL_LEN(counts);
    Zero(counts, size);
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->is_selected && unit->color == color)
        {
            const int32_t index = (int32_t) unit->trait.action + 1;
            counts[index]++;
        }
    }
    return (Action) (MaxIndex(counts, size) - 1);
}

static Type GetType(const Units units, const Color color)
{
    int32_t counts[TYPE_COUNT + 1];
    const int32_t size = UTIL_LEN(counts);
    Zero(counts, size);
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->is_selected && unit->color == color)
        {
            const int32_t index = (int32_t) unit->trait.type + 1;
            counts[index]++;
        }
    }
    return (Type) (MaxIndex(counts, size) - 1);
}

static Units UpdateAllMotives(Units units)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const Color color = (Color) i;
        static Motive zero;
        units.stamp[i].motive = zero;
        units.stamp[i].motive.action = GetAction(units, color);
        units.stamp[i].motive.type = GetType(units, color);
    }
    return units;
}

static int32_t CountPopulation(Units units, const Color color)
{
    int32_t count = 0;
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(!Unit_IsExempt(unit)
        && !unit->trait.is_inanimate
        && unit->color == color)
            count++;
    }
    return count;
}

static Units CountAllPopulations(Units units)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const Color color = (Color) i;
        units.stamp[i].status.population = CountPopulation(units, color);
    }
    return units;
}

static Units ButtonLookup(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map, const Button button, const Point cart, const bool is_floating)
{
    const Point zero = { 0,0 };
    const Parts parts = Parts_FromButton(button, overview.incoming.status.age, overview.incoming.status.civ);
    if(parts.part != NULL)
    {
        if(!Bits_Get(overview.incoming.bits, button.trigger))
        {
            const int32_t amount = button.icon_type == ICONTYPE_UNIT ? 10 : 1; // XXX: REMOVE IN FUTURE - THIS IS CRAZY.
            for(int32_t i = 0; i < amount; i++)
                units = SpawnParts(units, cart, zero, grid, overview.color, graphics, map, is_floating, parts, false, button.trigger);
        }
        Parts_Free(parts);
    }
    return units;
}

static Units UseIcon(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map, const bool is_floating)
{
    const Point cart = Overview_IsoToCart(overview, grid, overview.mouse_cursor, false);
    const Button button = Button_Upgrade(Button_FromOverview(overview), overview.incoming.bits);
    return ButtonLookup(units, overview, grid, graphics, map, button, cart, is_floating);
}

static Units SpawnUsingIcons(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map)
{
    return (overview.event.key_left_shift && overview.event.mouse_lu)
        ? UseIcon(units, overview, grid, graphics, map, false)
        : units;
}

static Units FloatUsingIcons(Units floats, const Overview overview, const Grid grid, const Registrar graphics, const Map map)
{
    return overview.event.key_left_shift
        ? UseIcon(floats, overview, grid, graphics, map, true)
        : floats;
}

static void PreservedUpgrade(Unit* const unit, const Grid grid, const Registrar graphics, const Graphics upgrade)
{
    static Unit zero;
    static Point none;
    const Point offset = unit->trait.is_inanimate ? none : unit->cart_grid_offset;
    Unit temp = zero;
    Unit_Preserve(&temp, unit);
    *unit = Unit_Make(unit->cart, offset, grid, upgrade, unit->color, graphics, false, false, TRIGGER_NONE);
    Unit_Preserve(unit, &temp);
}

static void UpgradeByBranch(Unit* const unit, const Overview overview, const Grid grid, const Registrar graphics)
{
    const Graphics next = unit->trait.upgrade;
    const Graphics tree = (Graphics) ((uint32_t) next + (uint32_t) overview.incoming.status.civ);
    const bool first_age = overview.incoming.status.age == AGE_1;
    const Graphics upgrade = first_age ? tree : next;
    PreservedUpgrade(unit, grid, graphics, upgrade);
}

static void AgeUpSimple(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Color color)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->color == color
        && unit->trait.upgrade != FILE_GRAPHICS_NONE
        && unit->trait.is_inanimate)
            UpgradeByBranch(unit, overview, grid, graphics);
    }
}

static Age GetNextAge(const Status status)
{
    return (Age) ((int32_t) status.age + 1);
}

static Units AgeUpBuildingByType(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map, const Button button, const Type type, const Color color)
{
    static Point zero;
    const Age age = GetNextAge(overview.incoming.status);
    const Parts parts = Parts_FromButton(button, age, overview.incoming.status.civ);
    Points points = Points_New(COLOR_COUNT);
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(Unit_IsType(unit, color, type))
        {
            const Point half = Point_Div(unit->trait.dimensions, 2);
            const Point cart = Point_Add(unit->cart, half);
            points = Points_Append(points, cart);
            Kill(units, unit);
            unit->must_skip_debris = true;
        }
    }
    for(int32_t i = 0; i < points.count; i++)
        units = SpawnParts(units, points.point[i], zero, grid, color, graphics, map, false, parts, true, TRIGGER_NONE);
    Points_Free(points);
    return units;
}

static Units UpgradeByType(const Units units, Unit* const flag, const Grid grid, const Registrar graphics, const Type type)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(Unit_IsType(unit, flag->color, type))
            PreservedUpgrade(unit, grid, graphics, unit->trait.upgrade);
    }
    return units;
}

// SOME BUILDINGS LIKE MILLS AND TOWNCENTERS SPAWN ADDITIONAL PARTS IN THEIR SUCCESSIVE AGES.
// THE SIMPLE AGE UP WILL NOT WORK. THE ENTIRE THING MUST BE REMOVED AND REPLACED.
static Units AgeUpAdvanced(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Color color, const Map map)
{
    typedef struct
    {
        Button button;
        Type type;
    }
    Print;
    const Print prints[] = {
        { { ICONTYPE_BUILD, { ICONBUILD_TOWN_CENTER }, TRIGGER_NONE }, TYPE_TOWN_CENTER },
        { { ICONTYPE_BUILD, { ICONBUILD_MILL        }, TRIGGER_NONE }, TYPE_MILL        },
    };
    for(int32_t i = 0; i < UTIL_LEN(prints); i++)
    {
        const Print print = prints[i];
        units = AgeUpBuildingByType(units, overview, grid, graphics, map, print.button, print.type, color);
    }
    return units;
}

static Units AgeUp(Units units, Unit* const flag, const Overview overview, const Grid grid, const Registrar graphics, const Map map)
{
    if(overview.color == flag->color)
        units.stamp[overview.color].status.age = GetNextAge(units.stamp[overview.color].status);
    AgeUpSimple(units, overview, grid, graphics, flag->color);
    return AgeUpAdvanced(units, overview, grid, graphics, flag->color, map);
}

static Units UpdateBits(Units units, const Overview overview, Unit* const flag)
{
    if(overview.color == flag->color)
        units.stamp[overview.color].bits = Bits_Set(units.stamp[overview.color].bits, flag->trigger);
    return units;
}

static Units TriggerTriggers(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const flag = &units.unit[i];
        if(Unit_IsTriggerValid(flag))
        {
            units = UpdateBits(units, overview, flag);
            flag->is_triggered = true;
            // SEE EARLY RETURN - ONLY ONE TRIGGER CAN RUN AT A TIME.
            switch(flag->trigger)
            {
            case TRIGGER_AGE_UP_2                     :
            case TRIGGER_AGE_UP_3                     :
            case TRIGGER_AGE_UP_4                     : return AgeUp(units, flag, overview, grid, graphics, map);
            case TRIGGER_UPGRADE_MILITIA              : return UpgradeByType(units, flag, grid, graphics, TYPE_MILITIA);
            case TRIGGER_UPGRADE_MAN_AT_ARMS          : return UpgradeByType(units, flag, grid, graphics, TYPE_MAN_AT_ARMS);
            case TRIGGER_UPGRADE_LONG_SWORDSMAN       : return UpgradeByType(units, flag, grid, graphics, TYPE_LONG_SWORDSMAN);
            case TRIGGER_UPGRADE_TWO_HANDED_SWORDSMAN : return UpgradeByType(units, flag, grid, graphics, TYPE_TWO_HANDED_SWORDSMAN);
            case TRIGGER_NONE                         : return units; // KEEP COMPILER QUIET.
            }
        }
    }
    return units;
}

static bool OutOfBounds(const Units units, const Point point)
{
    return point.x < 0
        || point.y < 0
        || point.x >= units.size
        || point.y >= units.size;
}

static Stack* GetStack(const Units units, const Point p)
{
    return &units.stack[p.x + p.y * units.size];
}

Stack Units_GetStackCart(const Units units, const Point p)
{
    static Stack zero;
    return OutOfBounds(units, p) ? zero : *GetStack(units, p);
}

static void ResetStacks(const Units units)
{
    for(int32_t y = 0; y < units.size; y++)
    for(int32_t x = 0; x < units.size; x++)
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

static void StackStacks(const Units units)
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

static void ManageStacks(const Units units)
{
    ResetStacks(units);
    StackStacks(units);
}

Units Units_Caretake(Units units, const Registrar graphics, const Grid grid, const Map map, const Field field)
{
    UpdateEntropy(units);
    Tick(units);
    units = ManagePathFinding(units, grid, map, field);
    units = UpdateAllMotives(units);
    Decay(units);
    Expire(units);
    units = KillTheDead(units, grid, graphics, map);
    units = RemoveGarbage(units);
    ManageStacks(units);
    units = CountAllPopulations(units);
    return units;
}

Units Units_Float(Units floats, const Units units, const Registrar graphics, const Overview overview, const Grid grid, const Map map, const Motive motive)
{
    floats.count = 0;
    floats.stamp[floats.color].status.age = units.stamp[units.color].status.age;
    floats.stamp[floats.color].motive = motive;
    ResetStacks(floats);
    floats = FloatUsingIcons(floats, overview, grid, graphics, map);
    StackStacks(floats);
    return floats;
}

static Units Service(Units units, const Registrar graphics, const Overview overview, const Grid grid, const Map map, const Field field)
{
    if(Overview_UsedAction(overview))
    {
        const Window window = Window_Make(overview, grid);
        units = Select(units, overview, grid, graphics, window.units);
        units = Command(units, overview, grid, graphics, map, field, window.units);
        units = SpawnUsingIcons(units, overview, grid, graphics, map);
        units = TriggerTriggers(units, overview, grid, graphics, map);
        Window_Free(window);
    }
    return units;
}

Units Units_PacketService(Units units, const Registrar graphics, const Packet packet, const Grid grid, const Map map, const Field field)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        units = Service(units, graphics, packet.overview[i], grid, map, field);
    return units;
}

uint64_t Units_Xor(const Units units)
{
    uint64_t parity = 0;
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        uint64_t xorred = Point_Flatten(unit->cell);
        xorred ^= (uint64_t) unit->id * (uint64_t) unit->color;
        xorred ^= (uint64_t) unit->id * (uint64_t) unit->dir;
        xorred ^= (uint64_t) unit->id * (uint64_t) unit->state;
        xorred ^= (uint64_t) unit->id * (uint64_t) unit->file;
        xorred ^= (uint64_t) unit->id * (uint64_t) unit->trigger;
        parity ^= xorred;
    }
    return parity;
}

static Unit* GetFirstTownCenter(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->color == units.color && unit->trait.type == TYPE_TOWN_CENTER)
            return unit;
    }
    return NULL;
}

Point Units_GetFirstTownCenterPan(const Units units, const Grid grid)
{
    static Point zero;
    Unit* const unit = GetFirstTownCenter(units);
    if(unit)
    {
        const Point cart = Unit_GetShift(unit, unit->cart);
        return Grid_CartToPan(grid, cart);
    }
    return zero;
}

void Units_ResetTiled(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].is_already_tiled = false;
}

static Units GenerateVillagers(Units units, const Map map, const Grid grid, const Registrar graphics, const Point slot, const Color color)
{
    static Point zero;
    const Button button = { ICONTYPE_UNIT, { ICONUNIT_MALE_VILLAGER }, TRIGGER_NONE };
    const Age age = units.stamp[color].status.age;
    const Civ civ = units.stamp[color].status.civ;
    const Parts villager = Parts_FromButton(button, age, civ);
    for(int32_t i = 0; i < CONFIG_UNITS_STARTING_VILLAGERS; i++)
    {
        const Point shift = { -2, 1 + i };
        const Point cart = Point_Add(slot, shift);
        units = SpawnParts(units, cart, zero, grid, color, graphics, map, false, villager, false, TRIGGER_NONE);
    }
    Parts_Free(villager);
    return units;
}

static Units GenerateStartingTrees(Units units, const Map map, const Grid grid, const Registrar graphics, const Point slot)
{
    static Point zero;
    const int32_t dis = CONFIG_UNITS_STARTING_TREE_TILES_FROM_TOWNCENTER;
    const Point point[] = {
        {  dis,    0 },
        {    0,  dis },
        {    0, -dis },
        { -dis,    0 },
    };
    const Parts parts = Parts_GetForestTree();
    for(int32_t i = 0; i < UTIL_LEN(point); i++)
    {
        const Point cart = Point_Add(slot, point[i]);
        Point shift;
        shift.x = Util_Rand() % (CONFIG_UNITS_STARTING_TREE_TILE_RANDOMNESS + 1);
        shift.y = Util_Rand() % (CONFIG_UNITS_STARTING_TREE_TILE_RANDOMNESS + 1);
        const Point shifted = Point_Add(cart, shift);
        units = SpawnParts(units, shifted, zero, grid, COLOR_GAIA, graphics, map, false, parts, false, TRIGGER_NONE);
    }
    return units;
}

static Units GenerateTownCenters(Units units, const Map map, const Grid grid, const Registrar graphics, const int32_t users)
{
    static Point zero;
    const Button button = { ICONTYPE_BUILD, { ICONBUILD_TOWN_CENTER }, TRIGGER_NONE };
    const Points points = Map_GetSlots(map);
    for(int32_t i = 0; i < users; i++)
    {
        const Color color = (Color) i;
        const Age age = units.stamp[color].status.age;
        const Civ civ = units.stamp[color].status.civ;
        const Parts towncenter = Parts_FromButton(button, age, civ);
        const int32_t index = (i * points.count) / users;
        const Point slot = points.point[index];
        const Point fixed = Map_GetFixedSlot(map, slot);
        units = SpawnParts(units, fixed, zero, grid, color, graphics, map, false, towncenter, false, TRIGGER_NONE);
        units = GenerateVillagers(units, map, grid, graphics, fixed, color);
        units = GenerateStartingTrees(units, map, grid, graphics, fixed);
        Parts_Free(towncenter);
    }
    Points_Free(points);
    return units;
}

Units Units_Generate(Units units, const Map map, const Grid grid, const Registrar graphics, const int32_t users)
{
    return (users > 0)
        ? GenerateTownCenters(units, map, grid, graphics, users)
        : units;
}

static void RestorePaths(const Units units, const Field field)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->must_repath_with_recover)
        {
            Unit_FindPath(unit, unit->cart_goal, unit->cart_grid_offset_goal, field);
            unit->must_repath_with_recover = false;
        }
    }
}

static Units UnpackRestore(Units units, const Restore restore)
{
    // GIVEN AN OLD STATE IS RESTORED, THE COMMAND GROUP MAY STAY THE SAME,
    // BUT INCREMENT FOR SAFETY AND GOOD MEASURE.
    units.command_group_next++;
    units.count = restore.count;
    units.repath_index = 0;
    Unit_SetIdNext(restore.id_next);
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i]  = restore.unit[i];
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        units.stamp[i] = restore.stamp[i];
    return units;
}

Units Units_ApplyRestore(Units units, const Restore restore, const Grid grid, const Field field)
{
    units = UnpackRestore(units, restore);
    // THE UNIT INTEREST POINTER WITHIN A UNIT NEEDS TO BE UPDATED ELSE
    // IT WILL POINT TO A SERVER MEMORY ADDRESS.
    // THE UNIT INTEREST POINTER RELIES ON THE STALE STACKS TO BE UPDATED,
    // SO THAT IS DONE FIRST.
    ManageStacks(units);
    DisengageAllBoids(units);
    EngageAllBoids(units, grid);
    RestorePaths(units, field);
    return RecountSelected(units);
}

Restore Units_PackRestore(const Units units, const int32_t cycles)
{
    static Restore zero;
    Restore restore = zero;
    restore.unit = units.unit;
    restore.count = units.count;
    restore.cycles = cycles;
    restore.id_next = Unit_GetIdNext();
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        restore.stamp[i] = units.stamp[i];
    return restore;
}
