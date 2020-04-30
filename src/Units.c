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
#include <limits.h>

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

void Units_Field(const Units units, const Map map, const Field field)
{
    for(int32_t row = 0; row < field.size; row++)
    for(int32_t col = 0; col < field.size; col++)
    {
        const Point point = { col, row };
        CanWalk(units, map, point)
            ? Field_Set(field, point, FIELD_WALKABLE_SPACE)
            : Field_Set(field, point, FIELD_OBSTRUCT_SPACE);
    }
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

Units Units_Make(const int32_t size, const int32_t cpu_count, const int32_t max, const Color color)
{
    static Units zero;
    Units units = zero;
    units.size = size;
    units.max = max;
    units.cpu_count = cpu_count;
    units.share[color].status.age = AGE_1;
    units.share[color].motive.action = ACTION_NONE;
    units.share[color].motive.type = TYPE_NONE;
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

static Units UnSelectAll(Units units, const Color color)
{
    Share share = units.share[color];
    share.select_count = 0;
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        Unit_ClearSelectedColor(unit, color);
    }
    units.share[color] = share;
    return units;
}

static Units SelectAll(Units units, const Color color)
{
    Share share = units.share[color];
    share.select_count = 0;
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(!Unit_IsExempt(unit) && unit->color == color)
        {
            Unit_SetSelectedColor(unit, color);
            share.select_count += 1;
        }
    }
    units.share[color] = share;
    return units;
}

static Units SelectAllForAllColors(Units units)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        units = SelectAll(units, (Color) i);
    return units;
}

static Units RecountSelected(Units units, const Color color)
{
    Share share = units.share[color];
    share.select_count = 0;
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(Unit_IsSelectedByColor(unit, color)
        && unit->color == color)
            share.select_count += 1;
    }
    units.share[color] = share;
    return units;
}

static Units Select(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Points render_points)
{
    if(overview.event.mouse_lu && !overview.event.key_left_alt)
    {
        const Tiles tiles = Tiles_PrepGraphics(graphics, overview, grid, units, render_points);
        Tiles_SortByHeight(tiles); // FOR SELECTING TRANSPARENT UNITS BEHIND INANIMATES OR TREES.
        units = UnSelectAll(units, overview.color);
        if(Overview_IsSelectionBoxBigEnough(overview))
            Tiles_SelectWithBox(tiles, overview.selection_box, overview.color);
        else
        {
            const Tile tile = Tiles_Get(tiles, overview, grid);
            if(tile.reference)
            {
                Tile_Select(tile, overview.color);
                if(overview.event.key_left_shift)
                    Tiles_SelectSimilar(tiles, tile, overview.color);
            }
        }
        Tiles_Free(tiles);
        return RecountSelected(units, overview.color);
    }
    return units;
}

static Unit* Last(const Units units)
{
    return &units.unit[units.count - 1];
}

static Units Append(Units units, const Unit unit)
{
    if(units.count == units.max)
        Util_Bomb("OUT OF MEMORY\n");
    units.unit[units.count++] = unit;
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

static void SafeStackAppend(const Units units, Unit* const unit, const Point cart)
{
    if(!OutOfBounds(units, cart))
        Stack_Append(GetStack(units, cart), unit);
}

Stack Units_GetStackCart(const Units units, const Point p)
{
    static Stack zero;
    return OutOfBounds(units, p)
        ? zero
        : *GetStack(units, p);
}

static int32_t GetStackCount(const Units units, const Point p)
{
    return Units_GetStackCart(units, p).count;
}

static void ProperStackAppend(const Units units, Unit* const unit)
{
    if(unit->trait.is_inanimate)
        for(int32_t y = 0; y < unit->trait.dimensions.y; y++)
        for(int32_t x = 0; x < unit->trait.dimensions.x; x++)
        {
            const Point point = { x, y };
            const Point cart = Point_Add(point, unit->cart);
            SafeStackAppend(units, unit, cart);
        }
    else SafeStackAppend(units, unit, unit->cart);
}

static Unit* FoundCreator(const Units units, Unit* const unit, const Color color) // XXX. CREATOR MUST HAVE LEAST AMOUNT OF CHILDREN.
{
    int32_t child_count_min = INT_MAX;
    Unit* out = NULL;
    for(int32_t j = 0; j < units.count; j++)
    {
        Unit* const parent = &units.unit[j];
        if(Unit_IsType(parent, color, unit->trait.creator))
        {
            const int32_t width = 2;
            const Rect rect = {{
                parent->cart.x - width,
                parent->cart.y - width,
            },{
                parent->cart.x + width + parent->trait.dimensions.x,
                parent->cart.y + width + parent->trait.dimensions.y,
            }};
            if(Rect_ContainsPoint(rect, unit->cart))
            {
                if(parent->child_count < child_count_min
                && Unit_IsSelectedByColor(parent, color)
                && parent->is_being_built == false)
                {
                    out = parent;
                    child_count_min = parent->child_count;
                }
            }
        }
    }
    return out;
}

static Units BulkAppend(Units units, const Map map, Unit* const temp, const Color color, const int32_t len, const bool ignore_collisions, const bool is_being_built)
{
    if(!ignore_collisions)
        for(int32_t i = 0; i < len; i++)
            if(!CanBuild(units, map, &temp[i]))
                return units;
    for(int32_t i = 0; i < len; i++)
    {
        Unit unit = temp[i];
        if(unit.trait.creator != TYPE_NONE && is_being_built)
        {
            Unit* const parent = FoundCreator(units, &unit, color);
            if(parent == NULL)
                return units;
            else
                Unit_SetParent(&unit, parent);
        }
        units = Append(units, unit);
        ProperStackAppend(units, Last(units));
    }
    return units;
}

static void SetChildren(Unit unit[], const int32_t count)
{
    if(count > 1)
    {
        Unit* const parent = &unit[0];
        parent->has_children = true;
        for(int32_t i = 1; i < count; i++)
        {
            Unit* const child = &unit[i];
            Unit_SetParent(child, parent);
        }
    }
}

static Units SpawnParts(Units units, const Point cart, const Point offset, const Grid grid, const Color color, const Registrar graphics, const Map map, const bool is_floating, const Parts parts, const bool ignore_collisions, const Trigger trigger, const bool is_being_built)
{
    Unit* const temp = UTIL_ALLOC(Unit, parts.count);
    for(int32_t i = 0; i < parts.count; i++)
    {
        const Part part = parts.part[i];
        const Point cart_part = Point_Add(cart, part.cart);
        temp[i] = Unit_Make(cart_part, offset, grid, part.file, color, graphics, true, is_floating, trigger, is_being_built);
    }
    SetChildren(temp, parts.count);
    units = BulkAppend(units, map, temp, color, parts.count, ignore_collisions, is_being_built);
    free(temp);
    return units;
}

static Units PlantRallyPoint(Units units, Unit* const unit, const Overview overview, const Registrar graphics, const Map map, const Point cart_goal, const Point cart_grid_offset_goal, const Grid grid)
{
    if(unit->rally)
        Unit_Flag(unit->rally);
    unit->cart_goal = cart_goal;
    unit->cart_grid_offset_goal = cart_grid_offset_goal;
    const Parts parts = Parts_GetWayPointFlag();
    units = SpawnParts(units, cart_goal, cart_grid_offset_goal, grid, overview.color, graphics, map, false, parts, true, TRIGGER_NONE, false);
    Unit* const rally = Last(units);
    Unit_SetRally(unit, rally);
    Unit_SetParent(rally, unit);
    return units;
}

typedef struct
{
    Point point;
    int32_t mag;
}
Mag;

static int32_t CompareByMag(const void* a, const void* b)
{
    Mag* const aa = (Mag*) a;
    Mag* const bb = (Mag*) b;
    const int32_t ma = aa->mag;
    const int32_t mb = bb->mag;
    return ma > mb;
}

static Point GetNextBestInanimateCoord(const Units units, Unit* const unit, const Grid grid, const Field field) // XXX. DEPENDING ON NUMBER OF UNITS SELECTED, RANDOMLY SPREAD OUT.
{
#define WIDTH   (4)
#define SIDES   (4)
#define CORNERS (4)
#define MAX (SIDES * WIDTH + CORNERS)
    Mag mags[MAX];
    for(int32_t i = 0; i < MAX; i++)
    {
        static Mag zero;
        mags[i] = zero;
    }
    const Unit* const interest = unit->interest;
    int32_t count = 0;
    const int32_t width = 1;
    for(int32_t x = -width; x < interest->trait.dimensions.x + width; x++)
    for(int32_t y = -width; y < interest->trait.dimensions.y + width; y++)
        if(x == -width
        || y == -width
        || x == interest->trait.dimensions.x
        || y == interest->trait.dimensions.y)
        {
            const Point shift = { x, y };
            const Point cart = Point_Add(interest->cart, shift);
            if(Field_IsWalkable(field, cart))
            {
                const Point cell = Grid_CartToCell(grid, cart);
                const Point diff = Point_Sub(unit->cell, cell);
                const Mag mag = { cart, Point_Mag(diff) };
                mags[count++] = mag;
            }
        }
    // OF THE CLOSEST FOUR SIDES, PICK THE TILE WITH LEAST AMOUNT OF VILLAGERS.
#define MIN (4)
    UTIL_SORT(mags, count, CompareByMag);
    Point out = { -1,-1 };
    int32_t min = INT_MAX;
    for(int32_t i = 0; i < SIDES; i++)
    {
        const Point cart = mags[i].point;
        const int32_t stacked = GetStackCount(units, cart);
        if(stacked < min)
        {
            min = stacked;
            out = cart;
        }
    }
    return out;
#undef WIDTH
#undef SIDES
#undef CORNERS
#undef MAX
#undef MIN
}

static void SetupGoal(const Units units, Unit* const unit, const Grid grid, const Field field)
{
    if(unit->interest->trait.is_inanimate) // PURSUE BUILDING NEXT BEST.
    {
        static Point zero;
        unit->cart_goal = GetNextBestInanimateCoord(units, unit, grid, field);
        unit->cart_grid_offset_goal = zero;
    }
    else // PURSUE ANIMATE.
    {
        unit->cart_goal = unit->interest->cart;
        unit->cart_grid_offset_goal = unit->interest->cart_grid_offset;
        if(unit->interest->color == unit->color)
            unit->command_group = unit->interest->command_group;
    }
}

static void FindPath(const Units units, Unit* const unit, const Point cart_goal, const Point cart_grid_offset_goal, const Grid grid, const Field field)
{
    if(!Unit_IsExempt(unit))
    {
        if(unit->interest)
            SetupGoal(units, unit, grid, field);
        else
        {
            unit->cart_goal = cart_goal;
            unit->cart_grid_offset_goal = cart_grid_offset_goal;
        }
        Unit_FreePath(unit);
        unit->has_direct = Unit_HasDirectPath(unit, grid, field);
        unit->path = unit->has_direct
            ? Points_PathStraight(unit->cart, unit->cart_goal)
            : Field_PathAStar(field, unit->cart, unit->cart_goal);
    }
}

static void Repath(const Units units, Unit* const unit, const Grid grid, const Field field)
{
    if(!Unit_IsExempt(unit)
    && unit->path_index_timer > CONFIG_UNIT_PATHING_TIMEOUT_CYCLES
    && Unit_HasPath(unit))
        FindPath(units, unit, unit->cart_goal, unit->cart_grid_offset_goal, grid, field);
}

static void AdvanceBuildAnimate(const Units units, Unit* const unit, const Grid grid, const Field field, const bool allowed_to_unlock_parent)
{
    unit->health += 1;
    if(unit->health >= unit->trait.max_health)
    {
        if(allowed_to_unlock_parent)
        {
            if(unit->parent->rally)
            {
                unit->command_group = Unit_GetCommandGroupNext();
                Unit_SetInterest(unit, unit->parent->interest);
                FindPath(units, unit, unit->parent->cart_goal, unit->parent->cart_grid_offset_goal, grid, field);
            }
            unit->parent->child_lock_id = -1;
            unit->parent->child_count -= 1;
        }
        unit->is_being_built = false;
        unit->is_floating = false;
        unit->has_parent_lock = false;
        Unit_SetParent(unit, NULL);
    }
}

static Units FindPathForSelected(Units units, const Overview overview, const Registrar graphics, const Map map, const Point cart_goal, const Point cart_grid_offset_goal, const Grid grid, const Field field)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->color == overview.color
        && Unit_IsSelectedByColor(unit, overview.color))
        {
            if(unit->trait.is_inanimate)
                units = PlantRallyPoint(units, unit, overview, graphics, map, cart_goal, cart_grid_offset_goal, grid);
            else
            {
                unit->command_group = Unit_GetCommandGroupNext();
                FindPath(units, unit, cart_goal, cart_grid_offset_goal, grid, field);
            }
        }
    }
    return units;
}

static void SetSelectedInterest(const Units units, const Color color, Unit* const interest)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(Unit_IsSelectedByColor(unit, color)
        && unit->color == color)
            Unit_SetInterest(unit, interest);
    }
}

static void SetSelectedAggroMove(const Units units, const Color color, const bool using_aggro_move)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(Unit_IsSelectedByColor(unit, color)
        && unit->color == color)
            unit->using_aggro_move = using_aggro_move;
    }
}

static void DisengageSelected(const Units units, const Color color)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(Unit_IsSelectedByColor(unit, color)
        && unit->color == color)
            unit->is_engaged_in_melee = false;
    }
}

static Units PathSelectedToUnit(const Units units, Unit* const unit, const Overview overview, const Grid grid, const Registrar graphics, const Map map, const Field field)
{
    unit->grid_flash_timer = 0;
    DisengageSelected(units, overview.color);
    SetSelectedInterest(units, overview.color, unit);
    return FindPathForSelected(units, overview, graphics, map, unit->cart, unit->cart_grid_offset, grid, field);
}

static Units MoveToNewConstruction(const Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map, const Field field)
{
    const Point cart_goal = Overview_IsoToCart(overview, grid, overview.mouse_cursor, false);
    const Stack stack = Units_GetStackCart(units, cart_goal);
    for(int32_t i = 0; i < stack.count; i++)
    {
        Unit* const reference = stack.reference[i];
        if(!Unit_IsExempt(reference))
            return PathSelectedToUnit(units, reference, overview, grid, graphics, map, field);
    }
    return units;
}

static Units MoveTo(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map, const Field field, const Points render_points)
{
    const Tiles tiles = Tiles_PrepGraphics(graphics, overview, grid, units, render_points);
    Tiles_SortByHeight(tiles); // FOR SELECTING TRANSPARENT UNITS BEHIND INANIMATES OR TREES.
    const Tile tile = Tiles_Get(tiles, overview, grid);
    if(tile.reference && !Unit_IsExempt(tile.reference) && !tile.reference->is_floating)
        units = PathSelectedToUnit(units, tile.reference, overview, grid, graphics, map, field);
    else
    {
        DisengageSelected(units, overview.color);
        SetSelectedInterest(units, overview.color, NULL);
        const Point cart_goal = Overview_IsoToCart(overview, grid, overview.mouse_cursor, false);
        const Point cart_grid = Overview_IsoToCart(overview, grid, overview.mouse_cursor, true);
        const Point cart_grid_offset_goal = Grid_GetOffsetFromGridPoint(grid, cart_grid);
        units = FindPathForSelected(units, overview, graphics, map, cart_goal, cart_grid_offset_goal, grid, field);
        const Parts parts = Parts_GetRedArrows();
        units = SpawnParts(units, cart_goal, cart_grid_offset_goal, grid, COLOR_GAIA, graphics, map, false, parts, false, TRIGGER_NONE, false);
    }
    Tiles_Free(tiles);
    return units;
}

static Units Command(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map, const Field field, const Points render_points)
{
    const Button button = Button_FromOverview(overview);
    const bool using_aggro_move = Button_UseAggroMove(button);
    const bool using_building_icon = button.icon_type == ICONTYPE_BUILD;
    if(overview.share.select_count > 0)
    {
        if(overview.event.mouse_lu && using_building_icon)
            units = MoveToNewConstruction(units, overview, grid, graphics, map, field);
        else
        if(overview.event.mouse_ru || using_aggro_move)
        {
            units = MoveTo(units, overview, grid, graphics, map, field, render_points);
            SetSelectedAggroMove(units, overview.color, using_aggro_move);
        }
        Unit_IncrementCommandGroup();
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
            const Point shift = { x, y };
            const Point cart = Point_Add(unit->cart, shift);
            const Stack stack = Units_GetStackCart(units, cart);
            for(int32_t i = 0; i < stack.count; i++)
            {
                Unit* const other = stack.reference[i];
                // FLOATING ANIMATES CAN CLIP FLOATING ANIMATES ONLY.
                // LIKEWISE, NON FLOATING ANIMATES CAN CLIP NON FLOATING ANIMATES ONLY.
                if(Unit_CanAnimateClipAnimate(unit, other))
                    continue;
                if(!other->trait.is_inanimate)
                {
                    const Point force = Unit_Separate(unit, other);
                    out = Point_Sub(out, force);
                }
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
            const Point shift = { x, y };
            const Point cart = Point_Add(unit->cart, shift);
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
        units = SpawnParts(units, cart, grid_offset, grid, COLOR_GAIA, graphics, map, false, parts, true, TRIGGER_NONE, false);
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
        units = SpawnParts(units, cart, zero, grid, COLOR_GAIA, graphics, map, false, parts, true, TRIGGER_NONE, false);
    }
    return units;
}

static void DisengageFrom(const Units units, Unit* const other)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->interest == other)
        {
            unit->is_engaged_in_melee = false;
            Unit_SetInterest(unit, NULL);
        }
    }
}

static void FlagChildren(const Units units, Unit* const unit)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const child = &units.unit[i];
        if(child->parent_id == unit->id)
            Unit_Flag(child);
    }
}

static void Flag(const Units units, Unit* const unit)
{
    Unit_Flag(unit);
    DisengageFrom(units, unit);
    if(unit->has_children)
        FlagChildren(units, unit);
}

static Units FlagTheDead(Units units, const Grid grid, const Registrar graphics, const Map map)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(!Unit_IsExempt(unit) && Unit_IsDead(unit))
        {
            Flag(units, unit);
            if(unit->must_skip_debris)
                continue;
            if(unit->trait.is_inanimate)
            {
                Unit_MakeRubble(unit, grid, graphics);
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
    const int32_t width = 2;
    Unit* closest = NULL;
    int32_t min = INT32_MAX;
    for(int32_t x = -width; x <= width; x++)
    for(int32_t y = -width; y <= width; y++)
    {
        const Point shift = { x, y };
        const Point cart = Point_Add(unit->cart, shift);
        const Stack stack = Units_GetStackCart(units, cart);
        for(int32_t i = 0; i < stack.count; i++)
        {
            Unit* const other = stack.reference[i];
            if(Unit_AreEnemies(unit, other))
            {
                const Point cell = other->trait.is_inanimate
                    ? Grid_CartToCell(grid, cart)
                    : other->cell;
                const Point diff = Point_Sub(cell, unit->cell);
                const int32_t mag = Point_Mag(diff);
                if(mag < min)
                {
                    min = mag;
                    closest = other;
                }
            }
        }
    }
    return closest;
}

static bool InterestInRange(const Units units, Unit* const unit)
{
    const int32_t width = 1;
    for(int32_t x = -width; x <= width; x++)
    for(int32_t y = -width; y <= width; y++)
    {
        const Point shift = { x, y };
        const Point cart = Point_Add(unit->cart, shift);
        const Stack stack = Units_GetStackCart(units, cart);
        for(int32_t i = 0; i < stack.count; i++)
        {
            Unit* const other = stack.reference[i];
            if(other == unit->interest)
                return true;
        }
    }
    return false;
}

static void EngageBoids(const Units units, Unit* const unit, const Grid grid)
{
    if(!Unit_IsExempt(unit) && !unit->is_state_locked)
    {
        if(unit->using_aggro_move)
        {
            Unit* const closest = GetClosestBoid(units, unit, grid);
            if(closest
            && closest->trait.type != TYPE_FLAG)
            {
                Unit_SetInterest(unit, closest);
                Unit_UpdateCellInterestInanimate(unit, grid);
                Unit_EngageWithMock(unit, closest, grid);
            }
        }
        else
        if(InterestInRange(units, unit))
        {
            Unit_UpdateCellInterestInanimate(unit, grid);
            Unit_EngageWithMock(unit, unit->interest, grid);
        }
    }
}

static void EngageAllBoids(const Units units, const Grid grid)
{
    for(int32_t i = 0; i < units.count; i++)
        EngageBoids(units, &units.unit[i], grid);
}

static void NullInterestPointers(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        unit->interest = NULL;
        unit->parent = NULL;
        unit->rally = NULL;
    }
}

// DOES NOT NEED TO BE THREADED -
// GIVES A MORE NATURAL FEEL TO HAVE THE PATHER REPATH IN TIME SLICES.
static Units RepathSomeBoids(Units units, const Grid grid, const Field field)
{
    int32_t slice = units.count / CONFIG_UNITS_REPATH_SLICE_SIZE;
    if(slice == 0)
        slice = units.count;
    int32_t end = slice + units.repath_index;
    if(end > units.count)
        end = units.count;
    for(int32_t i = units.repath_index; i < end; i++)
        Repath(units, &units.unit[i], grid, field);
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
        default:
            // THERE ARE ONLY 4 TYPES FOR ALL OF TIME. DEFAULT IS FINE.
            break;
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
            units.share[unit->color].status = Gather(units.share[unit->color].status, resource);
    }
    return units;
}

static Units ProcessHardRules(Units units, const Field field, const Grid grid)
{
    units = RepathSomeBoids(units, grid, field);
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
        unit->grid_flash_timer++;
        if(Unit_FlashTimerTick(unit))
            unit->is_flash_on ^= true;
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
            Unit_ClearSelectedAllColors(unit);
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

static int32_t CompareByGarbage(const void* a, const void* b)
{
    Unit* const aa = (Unit*) a;
    Unit* const bb = (Unit*) b;
    return aa->must_garbage_collect > bb->must_garbage_collect;
}

static Units Resize(Units units)
{
    UTIL_SORT(units.unit, units.count, CompareByGarbage);
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

static int32_t CompareById(const void* a, const void* b)
{
    Unit* const aa = (Unit*) a;
    Unit* const bb = (Unit*) b;
    return aa->id - bb->id;
}

static void ResetInterest(const Units units, Unit* const unit)
{
    if(unit->interest_id != -1)
    {
        Unit key;
        key.id = unit->interest_id;
        Unit* const found = UTIL_SEARCH(units.unit, Unit, units.count, &key, CompareById);
        if(found)
            unit->interest = found;
    }
}

static void ResetParent(const Units units, Unit* const unit)
{
    if(unit->parent_id != -1)
    {
        Unit key;
        key.id = unit->parent_id;
        Unit* const found = UTIL_SEARCH(units.unit, Unit, units.count, &key, CompareById);
        if(found)
            unit->parent = found;
    }
}

static void ResetRally(const Units units, Unit* const unit)
{
    if(unit->rally_id != -1)
    {
        Unit key;
        key.id = unit->rally_id;
        Unit* const found = UTIL_SEARCH(units.unit, Unit, units.count, &key, CompareById);
        if(found)
            unit->rally = found;
    }
}

typedef struct
{
    Units units;
    int32_t a;
    int32_t b;
}
ResetNeedle;

static int32_t RunResetNeedle(void* data)
{
    ResetNeedle* needle = (ResetNeedle*) data;
    for(int32_t i = needle->a; i < needle->b; i++)
    {
        Unit* const unit = &needle->units.unit[i];
        ResetInterest(needle->units, unit);
        ResetParent(needle->units, unit);
        ResetRally(needle->units, unit);
    }
    return 0;
}

static void ResetPointers(const Units units)
{
    UTIL_SORT(units.unit, units.count, CompareById);
    ResetNeedle* const needles = UTIL_ALLOC(ResetNeedle, units.cpu_count);
    const int32_t width = units.count / units.cpu_count;
    const int32_t remainder = units.count % units.cpu_count;
    for(int32_t i = 0; i < units.cpu_count; i++)
    {
        needles[i].units = units;
        needles[i].a = (i + 0) * width;
        needles[i].b = (i + 1) * width;
    }
    needles[units.cpu_count - 1].b += remainder;
    SDL_Thread** const threads = UTIL_ALLOC(SDL_Thread*, units.cpu_count);
    for(int32_t i = 0; i < units.cpu_count; i++) threads[i] = SDL_CreateThread(RunResetNeedle, "N/A", &needles[i]);
    for(int32_t i = 0; i < units.cpu_count; i++) SDL_WaitThread(threads[i], NULL);
    free(needles);
    free(threads);
}

static Units RemoveGarbage(Units units)
{
    FlagGarbage(units);
    units = Resize(units);
    NullInterestPointers(units);
    ResetPointers(units);
    return units;
}

static void UpdateEntropy(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
        units.unit[i].entropy = Point_Rand();
}

static Action GetAction(const Units units, const Color color)
{
    int32_t counts[ACTION_COUNT + 1];
    const int32_t size = UTIL_LEN(counts);
    Util_ZeroIntArray(counts, size);
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(Unit_IsSelectedByColor(unit, color)
        && unit->color == color
        && !unit->is_being_built)
        {
            const int32_t index = (int32_t) unit->trait.action + 1;
            counts[index]++;
        }
    }
    return (Action) (Util_MaxIntIndex(counts, size) - 1);
}

static Type GetType(const Units units, const Color color)
{
    int32_t counts[TYPE_COUNT + 1];
    const int32_t size = UTIL_LEN(counts);
    Util_ZeroIntArray(counts, size);
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(Unit_IsSelectedByColor(unit, color)
        && unit->color == color)
        {
            const int32_t index = (int32_t) unit->trait.type + 1;
            counts[index]++;
        }
    }
    return (Type) (Util_MaxIntIndex(counts, size) - 1);
}

static Units UpdateAllMotives(Units units)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const Color color = (Color) i;
        static Motive zero;
        units.share[i].motive = zero;
        units.share[i].motive.action = GetAction(units, color);
        units.share[i].motive.type = GetType(units, color);
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
        units.share[i].status.population = CountPopulation(units, color);
    }
    return units;
}

static Units SpawnWithButton(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map, const Button button, const Point cart, const bool is_floating, const bool is_being_built)
{
    const Point zero = { 0,0 };
    const Parts parts = Parts_FromButton(button, overview.share.status.age);
    if(parts.count > 0 )
    {
        // BITS ARE GOTTEN FROM UNITS BECAUSE TRIGGER SPEED WILL OUTMATCH OVERVIEW SPEED.
        if(!Bits_Get(units.share[overview.color].bits, button.trigger)
        && !Bits_Get(units.share[overview.color].busy, button.trigger))
        {
            const bool using_flag = parts.part[0].file == FILE_GRAPHICS_FLAG_TALL;
            const int32_t count =
                (!using_flag && overview.event.key_left_shift)
                    ? 3
                    : 1;
            for(int32_t i = 0; i < count; i++)
                units = SpawnParts(units, cart, zero, grid, overview.color, graphics, map, is_floating, parts, false, button.trigger, is_being_built);
        }
        Parts_Free(parts);
    }
    return units;
}

static Units UseIcon(Units units, const Overview overview, const Grid grid, const Registrar graphics, const Map map, const bool is_floating, const bool is_being_built)
{
    const Point cart = Overview_IsoToCart(overview, grid, overview.mouse_cursor, false);
    const Button button = Button_FromOverview(overview);
    const Button upgrade = Button_Upgrade(button, overview.share.bits);
    return SpawnWithButton(units, overview, grid, graphics, map, upgrade, cart, is_floating, is_being_built);
}

static Units AppendMissing(const Units units, Unit* const unit, const Grid grid, const Registrar graphics, const Graphics file)
{
    static Point zero;
    Unit missing = Unit_Make(unit->cart, zero, grid, file, unit->color, graphics, false, false, TRIGGER_NONE, unit->is_being_built);
    Unit_SetParent(&missing, unit);
    return Append(units, missing);
}

static Units UpgradeInanimate(Units units, Unit* const flag, const Grid grid, const Registrar graphics, const Age age)
{
    units.share[flag->color].status.age = age;
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->color == flag->color)
        {
            const Graphics upgrade = unit->trait.upgrade;
            if(upgrade != FILE_GRAPHICS_NONE && unit->trait.is_inanimate)
            {
                if(unit->file == FILE_GRAPHICS_AGE_1_MILL)
                    units = AppendMissing(units, unit, grid, graphics, FILE_GRAPHICS_AGE_2_WEST_EUROPE_MILL_SHADOW);
                Unit_PreservedUpgrade(unit, grid, graphics, upgrade);
            }
        }
    }
    return units;
}

static Units UpgradeType(const Units units, Unit* const flag, const Grid grid, const Registrar graphics, const Type type)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(Unit_IsType(unit, flag->color, type))
        {
            const Graphics upgrade = unit->trait.upgrade;
            if(upgrade != FILE_GRAPHICS_NONE)
                Unit_PreservedUpgrade(unit, grid, graphics, upgrade);
        }

    }
    return units;
}

static Units TriggerTriggers(Units units, const Grid grid, const Registrar graphics)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const flag = &units.unit[i];
        const Trigger trigger = flag->trigger;
        if(!flag->is_triggered && trigger != TRIGGER_NONE)
        {
            Bits* const bits = &units.share[flag->color].bits;
            Bits* const busy = &units.share[flag->color].busy;
            if(flag->is_being_built)
                *busy = Bits_Set(*busy, trigger);
            else
            {
                *bits = Bits_Set(*bits, trigger);
                flag->is_triggered = true;
                // ONLY ONE TRIGGER CAN RUN AT A TIME.
                switch(trigger)
                {
                case TRIGGER_AGE_UP_2: return UpgradeInanimate(units, flag, grid, graphics, AGE_2);
                case TRIGGER_AGE_UP_3: return UpgradeInanimate(units, flag, grid, graphics, AGE_3);
                case TRIGGER_UPGRADE_MILITIA:
                case TRIGGER_UPGRADE_MAN_AT_ARMS:
                case TRIGGER_UPGRADE_SPEARMAN:
                {
                    // TRIGGERS MAP 1:1 WITH TYPES FOR SIMPLE UPGRADES.
                    const Type type = (Type) trigger;
                    return UpgradeType(units, flag, grid, graphics, type);
                }
                case TRIGGER_NONE:
                    return units;
                }
            }
        }
    }
    return units;
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

static void StackStacks(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        ProperStackAppend(units, unit);
    }
}

static void ManageStacks(const Units units)
{
    ResetStacks(units);
    StackStacks(units);
}

static void CompleteInanimateConstruction(const Units units, const Map map)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->trait.is_inanimate)
        {
            if(unit->health >= unit->trait.max_health
            && unit->is_being_built)
            {
                DisengageFrom(units, unit);
                unit->is_being_built = false;
                if(unit->trait.type == TYPE_MILL)
                    Unit_LayFarm(unit, map);
            }
        }
    }
}

static void BuildAnimate(const Units units, const Grid grid, const Field field)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(!unit->trait.is_inanimate && unit->is_being_built)
        {
            Unit* const parent = unit->parent;
            if(parent)
            {
                if(parent->child_lock_id == -1)
                {
                    parent->child_lock_id = unit->id;
                    unit->has_parent_lock = true;
                }
                if(parent->child_lock_id == unit->id)
                    AdvanceBuildAnimate(units, unit, grid, field, true);
            }
            else
                AdvanceBuildAnimate(units, unit, grid, field, false);
        }
    }
}

static void Pursue(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(!Unit_IsExempt(unit)
        && unit->interest
        && unit->interest->trait.is_inanimate == false)
        {
            unit->cart_goal = unit->interest->cart;
            unit->cart_grid_offset_goal = unit->interest->cart_grid_offset;
        }
    }
}

Units Units_Caretake(Units units, const Registrar graphics, const Grid grid, const Map map, const Field field, const bool must_randomize_mouse)
{
    if(must_randomize_mouse)
        units = SelectAllForAllColors(units);
    UpdateEntropy(units);
    Tick(units);
    BuildAnimate(units, grid, field);
    CompleteInanimateConstruction(units, map);
    units = ManagePathFinding(units, grid, map, field);
    units = UpdateAllMotives(units);
    Decay(units);
    Expire(units);
    units = FlagTheDead(units, grid, graphics, map);
    units = RemoveGarbage(units);
    units = TriggerTriggers(units, grid, graphics);
    ManageStacks(units);
    Pursue(units);
    return CountAllPopulations(units);
}

Units Units_Float(Units floats, const Units units, const Registrar graphics, const Overview overview, const Grid grid, const Map map, const Motive motive)
{
    floats.count = 0;
    floats.share[floats.color].status.age = units.share[units.color].status.age;
    floats.share[floats.color].motive = motive;
    ResetStacks(floats);
    if(overview.event.key_left_alt)
        floats = UseIcon(floats, overview, grid, graphics, map, true, false);
    StackStacks(floats);
    return floats;
}

static Units Service(Units units, const Registrar graphics, const Overview overview, const Grid grid, const Map map, const Field field)
{
    if(Overview_UsedAction(overview))
    {
        const Window window = Window_Make(overview, grid);
        units = Select(units, overview, grid, graphics, window.units);
        if(overview.event.key_left_alt && overview.event.mouse_lu)
            units = UseIcon(units, overview, grid, graphics, map, false, true);
        units = Command(units, overview, grid, graphics, map, field, window.units);
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
        const uint64_t flat = Point_Flatten(unit->cell);
        const uint64_t xorred = flat + unit->id + unit->file + unit->health;
        parity ^= xorred;
    }
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const Status status = units.share[i].status;
        parity ^=
            ((uint64_t) (status.age       ) << 60) |
            ((uint64_t) (status.population) << 48) |
            ((uint64_t) (status.food      ) << 36) |
            ((uint64_t) (status.wood      ) << 24) |
            ((uint64_t) (status.gold      ) << 12) |
            ((uint64_t) (status.stone     ) <<  0);
    }
    return parity;
}

static Unit* GetFirstTownCenter(const Units units)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->color == units.color
        && unit->trait.type == TYPE_TOWN_CENTER)
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
    const Age age = units.share[color].status.age;
    const Parts villager = Parts_FromButton(button, age);
    for(int32_t i = 0; i < CONFIG_UNITS_STARTING_VILLAGERS; i++)
    {
        const Point shift = { -2, 1 + i };
        const Point cart = Point_Add(slot, shift);
        units = SpawnParts(units, cart, zero, grid, color, graphics, map, false, villager, false, TRIGGER_NONE, false);
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
        units = SpawnParts(units, shifted, zero, grid, COLOR_GAIA, graphics, map, false, parts, false, TRIGGER_NONE, false);
    }
    return units;
}

static Units GenerateTownCenters(Units units, const Map map, const Grid grid, const Registrar graphics, const int32_t users, const Color spectator)
{
    static Point zero;
    const Button button = { ICONTYPE_BUILD, { ICONBUILD_TOWN_CENTER }, TRIGGER_NONE };
    const Points points = Map_GetSlots(map);
    const int32_t players = users - 1; // DO NOT INCLUDE SPECTATOR.
    for(int32_t i = 0; i < players; i++)
    {
        const Color color = (Color) i;
        if(color != spectator)
        {
            const Age age = units.share[color].status.age;
            const Parts towncenter = Parts_FromButton(button, age);
            const int32_t index = (i * points.count) / players;
            const Point slot = points.point[index];
            units = SpawnParts(units, slot, zero, grid, color, graphics, map, false, towncenter, false, TRIGGER_NONE, false);
            units = GenerateVillagers(units, map, grid, graphics, slot, color);
            units = GenerateStartingTrees(units, map, grid, graphics, slot);
            Parts_Free(towncenter);
        }
    }
    Points_Free(points);
    return units;
}

Units Units_Generate(Units units, const Map map, const Grid grid, const Registrar graphics, const int32_t users, const Color spectator)
{
    return (users > 1)
        ? GenerateTownCenters(units, map, grid, graphics, users, spectator)
        : units;
}

static void RestorePaths(const Units units, const Grid grid, const Field field)
{
    for(int32_t i = 0; i < units.count; i++)
    {
        Unit* const unit = &units.unit[i];
        if(unit->must_repath_with_recover)
        {
            FindPath(units, unit, unit->cart_goal, unit->cart_grid_offset_goal, grid, field);
            unit->must_repath_with_recover = false;
        }
    }
}

static Units UnpackRestore(Units units, const Restore restore)
{
    if(restore.is_success)
    {
        units.count = restore.count;
        units.repath_index = 0;
        Unit_SetIdNext(restore.id_next);
        Unit_SetCommandGroupNext(restore.command_group_next);
        for(int32_t i = 0; i < units.count; i++)
        {
            Unit* const unit = &units.unit[i];
            *unit = restore.unit[i];
            unit->trait.file_name = Graphics_GetString(unit->file);
        }
        for(int32_t i = 0; i < COLOR_COUNT; i++)
            units.share[i] = restore.share[i];
    }
    else
        puts("RESTORE FAILURE, DID NOT GET ALL BYTES");
    return units;
}

Units Units_ApplyRestore(Units units, const Restore restore, const Grid grid, const Field field)
{
    units = UnpackRestore(units, restore);
    NullInterestPointers(units);
    ResetPointers(units);
    ManageStacks(units);
    EngageAllBoids(units, grid);
    RestorePaths(units, grid, field);
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        units = RecountSelected(units, (Color) i);
    return units;
}

Restore Units_PackRestore(const Units units, const int32_t cycles)
{
    static Restore zero;
    Restore restore = zero;
    restore.unit = units.unit;
    restore.count = units.count;
    restore.cycles = cycles;
    restore.id_next = Unit_GetIdNext();
    restore.command_group_next = Unit_GetCommandGroupNext();
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        restore.share[i] = units.share[i];
    return restore;
}
