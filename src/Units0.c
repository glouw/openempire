// Unit group pathfinding and actions.

#include "Units.h"

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

bool Units_CanBuild(const Units units, const Map map, Unit* const unit)
{
    if(unit->trait.can_expire)
        return true;
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
    field.rows = map.rows;
    field.cols = map.cols;
    field.object = UTIL_ALLOC(char, field.rows * field.cols);
    for(int32_t row = 0; row < field.rows; row++)
    for(int32_t col = 0; col < field.cols; col++)
    {
        const Point point = { col, row };
        CanWalk(units, map, point)
            ? Field_Set(field, point, FIELD_WALKABLE_SPACE)
            : Field_Set(field, point, FIELD_OBSTRUCT_SPACE);
    }
    return field;
}

Units Units_New(const Map map, const Overview overview, const Registrar graphics, const int32_t cpu_count)
{
    const int32_t max = CONFIG_UNITS_MAX;
    const int32_t area = overview.grid.rows * overview.grid.cols;
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
    units.rows = overview.grid.rows;
    units.cols = overview.grid.cols;
    units = Units_GenerateTestZone(units, map, overview, graphics);
    units.cpu_count = cpu_count;
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

static Units UnSelectAll(Units units)
{
    units.select_count = 0;
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].is_selected = false;
    return units;
}

static Units Select(Units units, const Overview overview, const Input input, const Registrar graphics, const Points render_points)
{
    if(input.lu && !input.key[SDL_SCANCODE_LSHIFT])
    {
        const Tiles tiles = Tiles_PrepGraphics(graphics, overview, units, render_points);
        Tiles_SortByHeight(tiles); // For selecting transparent units behind inanimates or trees.
        units = UnSelectAll(units);
        if(Overview_IsSelectionBoxBigEnough(overview))
            units.select_count = Tiles_SelectWithBox(tiles, overview.selection_box);
        else
        {
            const Tile tile = Tiles_SelectOne(tiles, input.point);
            if(tile.reference && tile.reference->is_selected)
                units.select_count = input.key[SDL_SCANCODE_LCTRL] ? Tiles_SelectSimilar(tiles, tile) : 1;
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
        if(unit->is_selected
#if !CONFIG_UNITS_GOD_MODE
        && unit->color == overview.color
#endif
        && unit->trait.max_speed > 0)
        {
            unit->command_group = units.command_group_next;
            unit->command_group_count = units.select_count;
            Unit_FindPath(unit, cart_goal, cart_grid_offset_goal, field);
        }
    }
}

static Units Command(Units units, const Overview overview, const Input input, const Registrar graphics, const Map map, const Field field)
{
    if(input.ru && units.select_count > 0)
    {
        const Point cart_goal = Overview_IsoToCart(overview, input.point, false);
        const Point cart = Overview_IsoToCart(overview, input.point, true);
        const Point cart_grid_offset_goal = Grid_GetOffsetFromGridPoint(overview.grid, cart);
        if(CanWalk(units, map, cart_goal))
        {
            units.command_group_next++;
            FindPathForSelected(units, overview, cart_goal, cart_grid_offset_goal, field);
            units = Units_Spawn(units, cart_goal, cart_grid_offset_goal, overview.grid, FILE_RIGHT_CLICK_RED_ARROWS, COLOR_GRY, graphics, map);
        }
    }
    return units;
}

static Point CoheseBoids(const Units units, Unit* const unit)
{
    static Point zero;
    if(!Unit_IsExempt(unit))
    {
        const Stack stack = Units_GetStackCart(units, unit->cart);
        const Point delta = Point_Sub(stack.center_of_mass, unit->cell);
        return Point_Div(delta, CONFIG_UNITS_COHESE_DIVISOR);
    }
    return zero;
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
    if(!Unit_IsExempt(unit)) // XXX. How to use normal vectors to run along walls?
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
        const int32_t repulsion = 1000; // XXX. How strong should this be?
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
            CoheseBoids(units, unit),
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
        case FILE_RUBBLE_1X1: return Point_Equal(_1x1_, dimensions);
        case FILE_RUBBLE_2X2: return Point_Equal(_2x2_, dimensions);
        case FILE_RUBBLE_3X3: return Point_Equal(_3x3_, dimensions);
        case FILE_RUBBLE_4X4: return Point_Equal(_4x4_, dimensions);
        case FILE_RUBBLE_5X5: return Point_Equal(_5x5_, dimensions);
    }
}

static Units SpamFire(Units units, Unit* const unit, const Grid grid, const Registrar graphics, const Map map)
{
    const Graphics fires[] = {
        FILE_FIRE_SMALL_A,
        FILE_FIRE_SMALL_B,
        FILE_FIRE_SMALL_C,
        FILE_FIRE_MEDIUM_A,
        FILE_FIRE_MEDIUM_B,
    };
    for(int32_t x = 1; x < unit->trait.dimensions.x - 1; x++)
    for(int32_t y = 1; y < unit->trait.dimensions.y - 1; y++)
    {
        const Point offset = { x, y };
        const Point cart = Point_Add(unit->cart, offset);
        const int32_t index = Util_Rand() % UTIL_LEN(fires);
        const int32_t w = grid.tile_cart_width;
        const int32_t h = grid.tile_cart_height;
        const Point grid_offset = {
            Util_Rand() % w - w / 2,
            Util_Rand() % h - h / 2,
        };
        units = Units_Spawn(units, cart, grid_offset, grid, fires[index], COLOR_GRY, graphics, map);
    }
    return units;
}

static Units SpamSmoke(Units units, Unit* const unit, const Grid grid, const Registrar graphics, const Map map)
{
    const Point none = { 0,0 };
    for(int32_t x = 0; x < unit->trait.dimensions.x; x++)
    for(int32_t y = 0; y < unit->trait.dimensions.y; y++)
    {
        const Point shift = { x, y };
        const Point cart = Point_Add(unit->cart, shift);
        units = Units_Spawn(units, cart, none, grid, FILE_SMALLER_EXPLOSION_SMOKE, COLOR_GRY, graphics, map);
    }
    return units;
}

void MakeRubble(Unit* unit, const Grid grid, const Registrar graphics)
{
    const Point none = { 0,0 };
    const Graphics rubbles[] = {
        FILE_RUBBLE_1X1,
        FILE_RUBBLE_2X2,
        FILE_RUBBLE_3X3,
        FILE_RUBBLE_4X4,
        FILE_RUBBLE_5X5,
    };
    Graphics file = (Graphics) FILE_NONE;
    for(int32_t i = 0; i < UTIL_LEN(rubbles); i++)
    {
        const Graphics rubble = rubbles[i];
        if(EqualDimension(unit->trait.dimensions, rubble))
            file = rubble;
    }
    if(file != FILE_NONE)
        *unit = Unit_Make(unit->cart, none, grid, file, unit->color, graphics);
}

static bool ShouldDelete(Unit* const unit, const Input input, const Overview overview)
{
    return
#if !CONFIG_UNITS_GOD_MODE
        unit->color == overview.color &&
#endif
        unit->is_selected && input.key[SDL_SCANCODE_DELETE];
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

static Units Kill(Units units, const Overview overview, const Registrar graphics, const Input input, const Map map)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(!Unit_IsExempt(unit))
            if(Unit_IsDead(unit) || ShouldDelete(unit, input, overview))
            {
                Unit_Kill(unit);
                if(unit->has_children)
                    KillChildren(units, unit);
                if(unit->trait.is_inanimate)
                {
                    MakeRubble(unit, overview.grid, graphics);
                    units = SpamFire(units, unit, overview.grid, graphics, map);
                    units = SpamSmoke(units, unit, overview.grid, graphics, map);
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
            if(other->color != unit->color && !Unit_IsExempt(other)) // XXX. USE ALLY SYSTEM INSTEAD OF COLOR FREE FOR ALL.
            {
                Point cell = other->cell;
                if(other->trait.is_inanimate)
                {
                    const Point mid = {
                        CONFIG_GRID_CELL_SIZE / 2,
                        CONFIG_GRID_CELL_SIZE / 2,
                    };
                    cell = Point_Add(cell, mid);
                }
                const Point diff = Point_Sub(cell, unit->cell);
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

static void EngageBoids(const Units units, Unit* const unit)
{
    if(!Unit_IsExempt(unit))
    {
        Unit* const closest = GetClosestBoid(units, unit);
        if(closest != NULL)
        {
            unit->is_engaged = true;
            unit->interest = closest;
            Unit_MockPath(unit, closest->cart, closest->cart_grid_offset);
        }
        else
        {
            unit->is_engaged = false;
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

static Units ProcessHardRules(Units units, const Field field, const Grid grid)
{
    units = Repath(units, field);
    for(int32_t i = 0; i < units.count; i++)
        ConditionallyStopBoids(units, &units.unit[i]);
    for(int32_t i = 0; i < units.count; i++)
        EngageBoids(units, &units.unit[i]);
    for(int32_t i = 0; i < units.count; i++)
    {
        const Resource resource = Unit_Melee(&units.unit[i], grid);
        if(resource.type != TYPE_NONE)
            switch(resource.type)
            {
            default:
                break;
            case TYPE_FOOD:
                units.food += resource.amount;
                break;
            case TYPE_WOOD:
                units.wood += resource.amount;
                break;
            case TYPE_GOLD:
                units.gold += resource.amount;
                break;
            case TYPE_STONE:
                units.stone += resource.amount;
                break;
            }
    }
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
        if(unit->timing_to_collect)
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

static int32_t CompareGarbage(const void* a, const void* b)
{
    Unit* const aa = (Unit*) a;
    Unit* const bb = (Unit*) b;
    return aa->must_garbage_collect > bb->must_garbage_collect;
}

static void SortGarbage(const Units units)
{
    qsort(units.unit, units.count, sizeof(*units.unit), CompareGarbage);
}

static void FlagGarbage(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        const int32_t last_tick = Unit_GetLastDecayTick(unit);
        if(unit->state == STATE_DECAY && unit->state_timer == last_tick)
            unit->must_garbage_collect = true;
        if(unit->timing_to_collect)
        {
            const int32_t time = (unit->trait.type == TYPE_FIRE) ? CONFIG_UNITS_CLEANUP_FIRE : CONFIG_UNITS_CLEANUP_RUBBLE;
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

static Units RemoveGarbage(const Units units)
{
    FlagGarbage(units);
    SortGarbage(units);
    return Resize(units);
}

static void UpdateEntropy(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        unit->entropy = Point_Rand();
    }
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

static Action GetAction(const Units units)
{
    int32_t counts[ACTION_COUNT + 1];
    const int32_t size = UTIL_LEN(counts);
    Zero(counts, size);
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
#if !CONFIG_UNITS_GOD_MODE
        if(unit->color == Color_GetMyColor())
#endif
        if(unit->is_selected)
        {
            const int32_t index = (int32_t) unit->trait.action + 1;
            counts[index]++;
        }
    }
    return (Action) (MaxIndex(counts, size) - 1);
}

static Type GetType(const Units units)
{
    int32_t counts[TYPE_COUNT + 1];
    const int32_t size = UTIL_LEN(counts);
    Zero(counts, size);
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
#if !CONFIG_UNITS_GOD_MODE
        if(unit->color == Color_GetMyColor())
#endif
        if(unit->is_selected)
        {
            const int32_t index = (int32_t) unit->trait.type + 1;
            counts[index]++;
        }
    }
    return (Type) (MaxIndex(counts, size) - 1);
}

static Units UpdateMotive(Units units)
{
    static Motive zero;
    units.motive = zero;
    units.motive.action = GetAction(units);
    units.motive.type = GetType(units);
    return units;
}

static Units CountPopulation(Units units)
{
    int32_t count = 0;
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(!Unit_IsExempt(unit)
        && unit->color == Color_GetMyColor()
        && !unit->trait.is_inanimate)
            count++;
    }
    units.population = count;
    return units;
}

static Units ServiceIcons(Units units, const Overview overview, const Registrar graphics, const Input input, const Map map)
{
    if(input.key[SDL_SCANCODE_LSHIFT] && input.lu)
    {
        const Point cart = Overview_IsoToCart(overview, input.point, false); // XXX. Use Color_GetMyColor
        const Icon icon = Icon_FromInput(input, units.motive);
        const Point none = { 0,0 };
        switch(icon)
        {
        case ICON_BUILD_HOUSE       : return Units_Spawn          (units, cart, none, overview.grid, FILE_DARK_AGE_HOUSE,                   overview.color, graphics, map);
        case ICON_BUILD_MILL        : return Units_SpawnWithShadow(units, cart,       overview.grid, FILE_DARK_AGE_MILL,                    overview.color, graphics, FILE_DARK_AGE_MILL_DONKEY, map);
        case ICON_BUILD_STONE_CAMP  : return Units_Spawn          (units, cart, none, overview.grid, FILE_NORTH_EUROPEAN_STONE_MINING_CAMP, overview.color, graphics, map);
        case ICON_BUILD_LUMBER_CAMP : return Units_Spawn          (units, cart, none, overview.grid, FILE_NORTH_EUROPEAN_LUMBER_CAMP,       overview.color, graphics, map);
        case ICON_BUILD_BARRACKS    : return Units_Spawn          (units, cart, none, overview.grid, FILE_DARK_AGE_BARRACKS,                overview.color, graphics, map);
        case ICON_BUILD_OUTPOST     : return Units_SpawnWithShadow(units, cart,       overview.grid, FILE_DARK_AGE_OUTPOST,                 overview.color, graphics, FILE_DARK_AGE_OUTPOST_SHADOW, map);
        case ICON_BUILD_TOWN_CENTER : return Units_SpawnTownCenter(units, cart,       overview.grid,                                        overview.color, graphics, map);
        case ICON_UNIT_MILITIA      : return Units_Spawn          (units, cart, none, overview.grid, FILE_MILITIA_IDLE,                     overview.color, graphics, map);
        case ICON_UNIT_MALE_VILLAGER: return Units_Spawn          (units, cart, none, overview.grid, FILE_MALE_VILLAGER_IDLE,               overview.color, graphics, map);
        default:
            break;
        }
    }
    return units;
}

static Units CheckIcons(Units units, const Overview overview, const Registrar graphics, const Input input, const Map map)
{
    return overview.color == Color_GetMyColor()
        ? ServiceIcons(units, overview, graphics, input, map)
        : units;
}

Units Units_Caretake(Units units, const Registrar graphics, const Overview overview, const Input input, const Map map, const Field field, const Window window)
{
    UpdateEntropy(units);
    Tick(units);
    units = CheckIcons(units, overview, graphics, input, map);
    units = ManagePathFinding(units, overview.grid, map, field);
    units = Select(units, overview, input, graphics, window.units);
    units = Command(units, overview, input, graphics, map, field);
    units = UpdateMotive(units);
    Decay(units);
    Expire(units);
    units = Kill(units, overview, graphics, input, map);
    units = RemoveGarbage(units);
    Units_ManageStacks(units);
    units = CountPopulation(units);
    return units;
}
