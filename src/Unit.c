#include "Unit.h"

#include "Rect.h"
#include "Util.h"
#include "Points.h"
#include "Resource.h"
#include "Config.h"

static int32_t id_next = 0;

static int32_t command_group_next = 0;

int32_t Unit_GetIdNext(void)
{
    return id_next;
}

void Unit_SetIdNext(const int32_t id)
{
    id_next = id;
}

int32_t Unit_GetCommandGroupNext(void)
{
    return command_group_next;
}

void Unit_SetCommandGroupNext(const int32_t command_group)
{
    command_group_next = command_group;
}

void Unit_IncrementCommandGroup(void)
{
    command_group_next += 1;
}

static void ConditionallySkipFirstPoint(Unit* const unit)
{
    if(unit->path.count > 1 && unit->path_index == 0)
        unit->path_index++;
}

static Point GetDelta(Unit* const unit, const Grid grid)
{
    static Point zero;
    const bool is_last = unit->path_index == unit->path.count - 1;
    const Point offset = is_last ? unit->cart_grid_offset_goal : zero;
    const Point cell_goal = Point_Add(
        Grid_OffsetToCell(offset),
        Grid_CartToCell(grid, unit->path.point[unit->path_index])
    );
    return Point_Sub(cell_goal, unit->cell);
}

void Unit_UpdatePathIndex(Unit* const unit, const int32_t index, const bool reset_path_index_timer)
{
    unit->path_index = index;
    if(reset_path_index_timer)
        unit->path_index_timer = 0;
}

void Unit_FreePath(Unit* const unit)
{
    Unit_UpdatePathIndex(unit, 0, true);
    unit->path = Points_Free(unit->path);
}

static void ReachGoal(Unit* const unit)
{
    Unit_UpdatePathIndex(unit, unit->path_index + 1, true);
    if(unit->path_index >= unit->path.count)
        Unit_FreePath(unit);
}

static void GotoGoal(Unit* const unit, const Point delta)
{
    static Point zero;
    const Point velocity = Point_Normalize(delta, unit->trait.max_speed);
    unit->velocity = Point_Div(Point_Add(velocity, unit->velocity), 2); // SMOOTHEN USING PREVIOUS AND NEW VELOCITY VECTORS.
    if(unit->state == STATE_ATTACK) // OVERRIDE AND SET TO ZERO IF ATTACKING.
        unit->velocity = zero;
    if(unit->is_engaged_in_melee)
    {
        const Point cell = unit->interest->trait.is_inanimate
            ? unit->cell_interest_inanimate
            : unit->interest->cell;
        const Point dir = Point_Sub(cell, unit->cell);
        Unit_SetDir(unit, dir);
    }
    else
        Unit_SetDir(unit, unit->velocity);
}

static void MoveAlongPath(Unit* const unit, const Grid grid)
{
    const Point delta = GetDelta(unit, grid);
    if(Point_Mag(delta) < CONFIG_POINT_GOAL_CLOSE_ENOUGH_MAG)
        ReachGoal(unit);
    else
        GotoGoal(unit, delta);
}

static void Stop(Unit* const unit)
{
    if(Point_Mag(unit->velocity) > 0)
    {
        static Point zero;
        unit->velocity = zero;
    }
}

static void FollowPath(Unit* const unit, const Grid grid)
{
    if(unit->path.count > 0)
    {
        ConditionallySkipFirstPoint(unit);
        MoveAlongPath(unit, grid);
    }
    else Stop(unit);
}

static void CapSpeed(Unit* const unit)
{
    if(Point_Mag(unit->velocity) > unit->trait.max_speed)
        unit->velocity = Point_Normalize(unit->velocity, unit->trait.max_speed);
}

void UpdateCart(Unit* const unit, const Grid grid)
{
    unit->cart_grid_offset = Grid_CellToOffset(grid, unit->cell);
    unit->cart = Grid_CellToCart(grid, unit->cell);
}

void Unit_UndoMove(Unit* const unit, const Grid grid)
{
    static Point zero;
    unit->cell = unit->cell_last;
    UpdateCart(unit, grid);
    unit->velocity = zero;
}

void Unit_Move(Unit* const unit, const Grid grid)
{
    unit->cell_last = unit->cell;
    unit->cell = Point_Add(unit->cell, unit->velocity);
    UpdateCart(unit, grid);
    if(unit->state != STATE_MOVE)
        Unit_SetState(unit, STATE_MOVE, false); // AN EXPENSIVE SWITCH CASE JUMP TO BE DOING EVERY FRAME.
    if(Point_Mag(unit->velocity) < CONFIG_UNIT_VELOCITY_DEADZONE)
        Unit_SetState(unit, STATE_IDLE, false);
}

void Unit_Lock(Unit* const unit)
{
    unit->is_state_locked = true;
}

void Unit_Unlock(Unit* const unit)
{
    unit->is_state_locked = false;
}

void Unit_SetState(Unit* const unit, const State state, const bool reset_state_timer)
{
    if(!unit->was_wall_pushed && !unit->is_state_locked && unit->trait.is_multi_state)
    {
        const Graphics file = Graphics_GetGraphicsState(unit->file, state);
        unit->state = state;
        unit->file = file;
        if(reset_state_timer)
            unit->state_timer = 0;
    }
}

static int32_t GetFramesFromState(Unit* const unit, const Registrar graphics, const State state)
{
    const Graphics file = Graphics_GetGraphicsState(unit->file, state);
    const Animation animation = graphics.animation[unit->color][file];
    return Animation_GetFramesPerDirection(animation);
}

static int32_t GetExpireFrames(Unit* const unit, const Registrar graphics)
{
    return graphics.animation[unit->color][unit->file].count;
}

Unit Unit_Make(const Point cart, const Point offset, const Grid grid, const Graphics file, const Color color, const Registrar graphics, const bool at_center, const bool is_floating, const Trigger trigger, const bool is_being_built)
{
    static Unit zero;
    Unit unit = zero;
    unit.trait = Trait_Build(file);
    unit.file = file;
    unit.id = id_next;
    Unit_IncrementCommandGroup();
    unit.command_group = Unit_GetCommandGroupNext();
    if(!is_floating)
        id_next++;
    unit.is_being_built = is_being_built;
    unit.is_floating = is_floating;
    if(!is_floating)
    {
        unit.entropy = Point_Rand();
        unit.entropy_static = Util_Rand();
    }
    if(is_being_built && !unit.trait.is_inanimate)
        unit.is_floating = true;
    unit.child_lock_id = -1;
    unit.parent_id = -1;
    unit.interest_id = -1;
    unit.rally_id = -1;
    unit.color = color;
    unit.state = STATE_IDLE;
#if 0
    unit.health = is_being_built ? 1 : unit.trait.max_health;
#else
    if(unit.trait.max_health > 0)
        unit.health = (9 * unit.trait.max_health) / 10;
#endif
    unit.trigger = trigger;
    Point temp = cart;
    if(at_center)
    {
        const Point center = Point_Div(unit.trait.dimensions, 2);
        temp = Point_Sub(temp, center);
    }
    unit.cell = Grid_CartToCell(grid, temp);
    if(Point_IsEven(unit.trait.dimensions))
    {
        const Point shift = {
            grid.tile_cart_mid.x * CONFIG_GRID_CELL_SIZE,
            grid.tile_cart_mid.y * CONFIG_GRID_CELL_SIZE,
        };
        unit.cell = Point_Sub(unit.cell, shift);
    }
    unit.cell = Point_Add(unit.cell, Grid_OffsetToCell(offset));
    UpdateCart(&unit, grid);
    if(unit.trait.is_multi_state)
    {
        // BIG PERFORMANCE GAIN FROM CACHING SINCE GRAPHICS_GETGRAPHICSSTATE IS NOT BEING CALLED.
        unit.attack_frames_per_dir = GetFramesFromState(&unit, graphics, STATE_ATTACK);
        unit.fall_frames_per_dir   = GetFramesFromState(&unit, graphics, STATE_FALL);
        unit.decay_frames_per_dir  = GetFramesFromState(&unit, graphics, STATE_DECAY);
    }
    if(unit.trait.can_expire)
        unit.expire_frames = GetExpireFrames(&unit, graphics);
    if(unit.trait.type == TYPE_FIRE
    || unit.trait.type == TYPE_RUBBLE)
        unit.is_timing_to_collect = true;
    return unit;
}

void Unit_Print(Unit* const unit)
{
    if(unit)
    {
        printf("command_group         :: %d\n", unit->command_group);
        printf("path.count            :: %d\n", unit->path.count);
        printf("child_count           :: %d\n", unit->child_count);
        printf("has_children          :: %d\n", unit->has_children);
        printf("string                :: %s\n", Graphics_GetString(unit->file));
        printf("has_direct            :: %d\n", unit->has_direct);
        printf("must_garbage_collect  :: %d\n", unit->must_garbage_collect);
        printf("health                :: %d\n", unit->health);
        printf("max_health            :: %d\n", unit->trait.max_health);
        printf("is_being_built        :: %d\n", unit->is_being_built);
        printf("self                  :: 0x%16p\n", (void*) unit);
        printf("interest              :: 0x%16p\n", (void*) unit->interest);
        printf("cart                  :: %d %d\n", unit->cart.x, unit->cart.y);
        printf("cart_grid_offset      :: %d %d\n", unit->cart_grid_offset.x, unit->cart_grid_offset.y);
        printf("cart_goal             :: %d %d\n", unit->cart_goal.x, unit->cart_goal.y);
        printf("cart_grid_offset_goal :: %d %d\n", unit->cart_grid_offset_goal.x, unit->cart_grid_offset_goal.y);
        printf("cell                  :: %d %d\n", unit->cell.x, unit->cell.y);
        printf("cell_inanimate        :: %d %d\n", unit->cell_interest_inanimate.x, unit->cell_interest_inanimate.y);
        printf("dim.x, dim.y          :: %d %d\n", unit->trait.dimensions.x, unit->trait.dimensions.y);
        printf("\n");
    }
}

void ApplyStressors(Unit* const unit)
{
    const int32_t resistance = 3; // XXX. HOW ABOUT A RESISTANCE TRAIT ADDED TO THIS BASE RESISTANCE? EG. HORSES HAVE MORE RESISTANCE?
    const Point stressors = (unit->state == STATE_ATTACK)
        ? Point_Div(unit->stressors, resistance)
        : unit->stressors;
    unit->velocity = Point_Add(unit->velocity, stressors);
}

void Unit_Flow(Unit* const unit, const Grid grid)
{
    FollowPath(unit, grid);
    ApplyStressors(unit);
    CapSpeed(unit);
}

static bool SameColor(Unit* const unit, Unit* const other)
{
    return unit->color == other->color;
}

bool Unit_InPlatoon(Unit* const unit, Unit* const other)
{
    return unit->command_group == other->command_group && SameColor(unit, other);
}

void Unit_SetDir(Unit* const unit, const Point dir)
{
    if(unit->dir_timer > CONFIG_UNIT_DIRECTION_TIMER_EXPIRE)
    {
        unit->dir = Direction_CartToIso(Direction_GetCart(dir));
        unit->dir_timer = 0;
    }
}

bool Unit_HasDirectPath(Unit* const unit, const Grid grid, const Field field)
{
    if(Point_Equal(unit->cart, unit->cart_goal))
        return true;
    const Point dir = Point_Sub(
        Point_Add(
            Grid_CartToCell(grid, unit->cart_goal),
            Grid_OffsetToCell(unit->cart_grid_offset_goal)),
        unit->cell);
    const int32_t step = CONFIG_GRID_CELL_SIZE;
    const Point mag = Point_Normalize(dir, step);
    Point cell = unit->cell;
    while(true)
    {
        const Point cart = Grid_CellToCart(grid, cell);
        if(!Field_IsWalkable(field, cart))
            return false;
        if(Point_Equal(cart, unit->cart_goal))
            return true;
        cell = Point_Add(cell, mag);
    }
    return false;
}

void Unit_MockPath(Unit* const unit, const Point cart_goal, const Point cart_grid_offset_goal)
{
    if(!Unit_IsExempt(unit))
    {
        Unit_FreePath(unit);
        unit->cart_goal = cart_goal;
        unit->cart_grid_offset_goal = cart_grid_offset_goal;
        unit->path = Points_PathStraight(unit->cart, cart_goal);
    }
}

void Unit_Flag(Unit* const unit)
{
    unit->health = 0;
    unit->is_being_built = false;
    Unit_ClearSelectedAllColors(unit);
    if(unit->trait.is_inanimate)
        unit->must_garbage_collect = true;
    else
    {
        Unit_Unlock(unit);
        Unit_SetState(unit, STATE_FALL, true);
    }
}

int32_t Unit_GetLastExpireTick(Unit* const unit)
{
    return unit->expire_frames * CONFIG_ANIMATION_DIVISOR - 1;
}

int32_t Unit_GetLastAttackTick(Unit* const unit)
{
    return unit->attack_frames_per_dir * CONFIG_ANIMATION_DIVISOR - 1;
}

int32_t Unit_GetLastDecayTick(Unit* const unit)
{
    return unit->decay_frames_per_dir * CONFIG_ANIMATION_DECAY_DIVISOR - 1;
}

int32_t Unit_GetLastFallTick(Unit* const unit)
{
    return unit->fall_frames_per_dir * CONFIG_ANIMATION_DIVISOR - 1;
}

static bool CanEngage(Unit* const unit, const Grid grid)
{
    const Point diff = Point_Sub(
        unit->interest->trait.is_inanimate
            ? unit->cell_interest_inanimate
            : unit->interest->cell,
        unit->cell);
    const int32_t width = UTIL_MAX(unit->trait.width, unit->interest->trait.width);
    if(unit->interest->trait.is_inanimate)
    {
        const bool is_mill_built = unit->interest->trait.type == TYPE_MILL && !unit->interest->is_being_built;
        const int32_t reach = (width / 2) + (is_mill_built ? CONFIG_UNIT_FARM_HARVEST_LENGTH : CONFIG_UNIT_SWORD_LENGTH);
        const Point feeler = Point_Normalize(diff, reach);
        const Point cell = Point_Add(unit->cell, feeler);
        const Point cart = Grid_CellToCart(grid, cell);
        const Point a = unit->interest->cart;
        const Point b = Point_Add(a, unit->interest->trait.dimensions);
        const Rect rect = { a, b };
        return Rect_ContainsPoint(rect, cart);
    }
    else
    {
        const int32_t reach = CONFIG_UNIT_SWORD_LENGTH + width;
        return Point_Mag(diff) < reach;
    }
}

static Resource Collect(Unit* const unit, const int amount)
{
    Resource resource;
    resource.amount = amount;
    switch(unit->trait.type)
    {
    case TYPE_TREE       : resource.type = TYPE_WOOD;  break;
    case TYPE_STONE_MINE : resource.type = TYPE_STONE; break;
    case TYPE_GOLD_MINE  : resource.type = TYPE_GOLD;  break;
    case TYPE_BERRY_BUSH : resource.type = TYPE_FOOD;  break;
    default:
        // DEFAULT IS FINE. BY LAW THERE ARE ONLY 4 RESOURCES TYPES IN AGE II.
        break;
    }
    unit->health -= amount;
    return resource;
}

static bool MustDisengage(Unit* const unit)
{
    return unit->state == STATE_ATTACK
        && unit->state_timer >= Unit_GetLastAttackTick(unit)
        && !Unit_IsDead(unit->interest);
}

static Resource DoVillagerAction(Unit* const unit)
{
    if(SameColor(unit, unit->interest))
    {
        if(Unit_IsConstruction(unit->interest))
            unit->interest->health += unit->trait.attack;
        else
        if(unit->interest->trait.type == TYPE_MILL)
        {
            const Resource resource = { TYPE_FOOD, unit->trait.attack };
            return resource;
        }
    }
    else
    if(unit->interest->trait.is_resource)
    {
        const Resource resource = Collect(unit->interest, unit->trait.attack);
        return resource;
    }
    else
        unit->interest->health -= unit->trait.attack;
    return Resource_None();
}

Resource Unit_Melee(Unit* const unit, const Grid grid)
{
    if(unit->interest != NULL
    && !Unit_IsExempt(unit)
    && !unit->is_floating
    && !Unit_IsExempt(unit->interest)
    && !unit->interest->is_floating
    && Unit_IsDifferent(unit, unit->interest)   // DO NOT MELEE SELF.
    && unit->interest->trait.type != TYPE_FLAG) // DO NOT ATTACK FLAGS THAT ARE RESEARCHING THINGS.
    {
        if(unit->using_aggro_move && unit->interest->trait.is_resource)
            goto out;
        if(CanEngage(unit, grid))
        {
            if((SameColor(unit, unit->interest) && unit->interest->is_being_built && Unit_IsVillager(unit))
            || (SameColor(unit, unit->interest) && unit->interest->trait.type == TYPE_MILL)
            || !SameColor(unit, unit->interest))
            {
                unit->is_engaged_in_melee = true;
                Unit_SetState(unit, STATE_ATTACK, true);
                Unit_Lock(unit);
            }
            else
                Unit_FreePath(unit);
        }
        if(MustDisengage(unit))
        {
            Unit_Unlock(unit);
            if(CanEngage(unit, grid)) // RANGE CHECK IS DONE AGAIN, ELSE UNIT CAN ATTACK INFINITELY FAR AWAY WITH QUICK CHANGE OF INTEREST.
            {
                if(Unit_IsVillager(unit))
                    return DoVillagerAction(unit);
                else
                    unit->interest->health -= unit->trait.attack;
            }
        }
    }
    else
        Unit_Unlock(unit);
out:return Resource_None();
}

static Point Nudge(Unit* const unit)
{
    const int32_t mag = UINT16_MAX;
    const Point half = { mag / 2, mag / 2 };
    return Point_Mul(Point_Sub(unit->entropy, half), mag);
}

Point Unit_Separate(Unit* const unit, Unit* const other)
{
    static Point zero;
    if(!Unit_IsExempt(other) && Unit_IsDifferent(unit, other))
    {
        const Point diff = Point_Sub(other->cell, unit->cell);
        if(Point_IsZero(diff))
            return Nudge(unit);
        const int32_t width = UTIL_MAX(unit->trait.width, other->trait.width);
        if(Point_Mag(diff) < width)
            return Point_Sub(Point_Normalize(diff, width), diff);
    }
    return zero;
}

bool Unit_IsDead(Unit* const unit)
{
    return unit->health <= 0;
}

bool Unit_IsExempt(Unit* const unit)
{
    return State_IsDead(unit->state)
        || unit->trait.is_detail
        || unit->must_garbage_collect;
}

Point Unit_GetShift(Unit* const unit, const Point cart)
{
    const Point shift = { 0, 1 };
    const Point half = Point_Div(unit->trait.dimensions, 2);
    return Point_Add(cart, Point_Add(shift, half));
}

bool Unit_IsDifferent(Unit* const unit, Unit* const other)
{
    return unit->id != other->id;
}

bool Unit_HasNoPath(Unit* const unit)
{
    return unit->path.count == 0;
}

bool Unit_HasPath(Unit* const unit)
{
    return unit->path.count > 0;
}

bool Unit_IsType(Unit* const unit, const Color color, const Type type)
{
    return unit->color == color && !Unit_IsExempt(unit) && unit->trait.type == type;
}

void Unit_Preserve(Unit* const to, const Unit* const from)
{
#define COPY(a, b, what) to->what = from->what
    COPY(to, from, cell);
    COPY(to, from, cart);
    COPY(to, from, is_selected_by);
    COPY(to, from, cart_grid_offset);
    COPY(to, from, id);
    COPY(to, from, rally_id);
    COPY(to, from, parent_id);
    COPY(to, from, interest_id);
    COPY(to, from, child_lock_id);
    // NOT NECESSARY TO COPY POINTERS OVER SINCE IDS WILL REMAP POINTERS.
    COPY(to, from, has_parent_lock);
    COPY(to, from, has_children);
    COPY(to, from, path);
    COPY(to, from, dir);
    COPY(to, from, health);
    COPY(to, from, using_aggro_move);
    COPY(to, from, cart_goal);
    COPY(to, from, cart_grid_offset_goal);
    COPY(to, from, is_being_built);
    COPY(to, from, is_floating);
#undef COPY
}

static void MorphState(Unit* const unit, const Graphics file)
{
    unit->file = file;
    unit->state = STATE_IDLE;
    unit->state_timer = 0;
    unit->is_state_locked = false;
}

static void Morph(Unit* const unit)
{
#define BRANCH(GENDER)                                                       \
    if(unit->interest->is_being_built)                                       \
        MorphState(unit, FILE_GRAPHICS_##GENDER##_VILLAGER_BUILDER_IDLE);    \
    else                                                                     \
    if(unit->interest->trait.type == TYPE_TREE)                              \
        MorphState(unit, FILE_GRAPHICS_##GENDER##_VILLAGER_WOODCUTTER_IDLE); \
    else                                                                     \
    if(unit->interest->trait.type == TYPE_BERRY_BUSH)                        \
        MorphState(unit, FILE_GRAPHICS_##GENDER##_VILLAGER_FORAGER_IDLE);    \
    else                                                                     \
    if(unit->interest->trait.type == TYPE_MILL)                              \
        MorphState(unit, FILE_GRAPHICS_##GENDER##_VILLAGER_FORAGER_IDLE);    \
    else                                                                     \
    if(unit->interest->trait.type == TYPE_STONE_MINE                         \
    || unit->interest->trait.type == TYPE_GOLD_MINE)                         \
        MorphState(unit, FILE_GRAPHICS_##GENDER##_VILLAGER_MINER_IDLE);      \
    else                                                                     \
        MorphState(unit, FILE_GRAPHICS_##GENDER##_VILLAGER_IDLE);
    if(unit->trait.type == TYPE_VILLAGER_MALE)
    {
        BRANCH(MALE)
    }
    if(unit->trait.type == TYPE_VILLAGER_FEMALE)
    {
        BRANCH(FEMALE)
    }
#undef BRANCH
}

void Unit_SetInterest(Unit* const unit, Unit* const interest)
{
    if(interest != unit)
    {
        unit->interest = interest;
        if(interest)
        {
            unit->interest_id = interest->id;
            Morph(unit);
        }
        else
            unit->interest_id = -1;
    }
}

void Unit_SetRally(Unit* const unit, Unit* const rally)
{
    if(rally != unit)
    {
        unit->rally = rally;
        if(rally)
            unit->rally_id = rally->id;
        else
            unit->rally_id = -1;
    }
}

bool Unit_FlashTimerTick(Unit* const unit)
{
    return (unit->grid_flash_timer % CONFIG_VRAM_FLASH_TIMER_RATE) == 0;
}

Graphics Unit_GetConstructionFile(Unit* const unit)
{
    const Graphics constructions[] = {
        FILE_GRAPHICS_CONSTRUCTION_1X1,
        FILE_GRAPHICS_CONSTRUCTION_2X2,
        FILE_GRAPHICS_CONSTRUCTION_3X3,
        FILE_GRAPHICS_CONSTRUCTION_4X4,
    };
    for(int32_t i = 0; i < UTIL_LEN(constructions); i++)
    {
        const Graphics construction = constructions[i];
        if(Graphics_EqualDimension(construction, unit->trait.dimensions))
            return construction;
    }
    return FILE_GRAPHICS_NONE;
}

void Unit_SetParent(Unit* const child, Unit* const parent)
{
    child->parent = parent;
    if(parent)
    {
        parent->child_count += 1;
        parent->has_children = true;
        child->parent_id = parent->id;
    }
    else
        child->parent_id = -1;
}

bool Unit_IsConstruction(Unit* const unit)
{
    return unit->trait.is_inanimate && unit->is_being_built;
}

bool Unit_AreEnemies(Unit* const unit, Unit* const other)
{
    return !SameColor(unit, other) && !Unit_IsExempt(other); // XXX: USE ALLY SYSTEM INSTEAD OF COLOR FREE FOR ALL.
}

void Unit_LayFarm(Unit* const unit, const Map map)
{
    const int32_t width = 2;
    for(int32_t x = -width; x < unit->trait.dimensions.x + width; x++)
    for(int32_t y = -width; y < unit->trait.dimensions.y + width; y++)
        if(x < 0
        || y < 0
        || x >= unit->trait.dimensions.x
        || y >= unit->trait.dimensions.y)
        {
            const Point shift = { x, y };
            const Point cart = Point_Add(unit->cart, shift);
            Map_SetTerrainFile(map, cart, FILE_TERRAIN_FARM_READY);
        }
}

static bool IsFloatingDifferent(Unit* const unit, Unit* const other)
{
    return unit->is_floating && !other->is_floating;
}

bool Unit_CanAnimateClipAnimate(Unit* const unit, Unit* const other)
{
    return IsFloatingDifferent(other, unit)
        || IsFloatingDifferent(unit, other);
}

bool Unit_IsSelectedByColor(Unit* const unit, const Color color)
{
    return (unit->is_selected_by >> color) & 0x1;
}

void Unit_ClearSelectedColor(Unit* const unit, const Color color)
{
    unit->is_selected_by &= ~(0x1 << color);
}

void Unit_ClearSelectedAllColors(Unit* const unit)
{
    unit->is_selected_by = 0x0;
}

void Unit_SetSelectedColor(Unit* const unit, const Color color)
{
    unit->is_selected_by |= 0x1 << color;
}

bool Unit_IsPointWithinDimensions(Unit* const unit, const Point cart)
{
    const Rect rect = {
        unit->cart,
        Point_Add(unit->cart, unit->trait.dimensions)
    };
    return Rect_ContainsPoint(rect, cart);
}

void Unit_MakeRubble(Unit* unit, const Grid grid, const Registrar graphics)
{
    static Point none;
    const Graphics rubbles[] = {
        FILE_GRAPHICS_RUBBLE_1X1,
        FILE_GRAPHICS_RUBBLE_2X2,
        FILE_GRAPHICS_RUBBLE_3X3,
        FILE_GRAPHICS_RUBBLE_4X4,
    };
    Graphics file = FILE_GRAPHICS_NONE;
    for(int32_t i = 0; i < UTIL_LEN(rubbles); i++)
    {
        const Graphics rubble = rubbles[i];
        if(Graphics_EqualDimension(rubble, unit->trait.dimensions))
            file = rubble;
    }
    if(file != FILE_GRAPHICS_NONE)
        *unit = Unit_Make(unit->cart, none, grid, file, unit->color, graphics, false, false, TRIGGER_NONE, false);
}

void Unit_UpdateCellInterestInanimate(Unit* const unit, const Grid grid)
{
    Unit* const interest = unit->interest;
    if(interest->trait.is_inanimate)
    {
        int32_t min = INT32_MAX;
        for(int32_t x = 0; x < interest->trait.dimensions.x; x++)
        for(int32_t y = 0; y < interest->trait.dimensions.y; y++)
        {
            const Point shift = { x, y };
            const Point cart = Point_Add(interest->cart, shift);
            const Point cell = Grid_CartToCell(grid, cart);
            const Point diff = Point_Sub(unit->cell, cell);
            const int32_t mag = Point_Mag(diff);
            if(mag < min)
            {
                min = mag;
                unit->cell_interest_inanimate = cell;
            }
        }
    }
}

void Unit_EngageWithMock(Unit* const unit, Unit* const closest, const Grid grid)
{
    static Point zero;
    Point cart = zero;
    Point offset = zero;
    if(closest->trait.is_inanimate)
        cart = Grid_CellToCart(grid, unit->cell_interest_inanimate);
    else
    {
        cart = closest->cart;
        offset = closest->cart_grid_offset;
    }
    Unit_MockPath(unit, cart, offset);
}

void Unit_PreservedUpgrade(Unit* const unit, const Grid grid, const Registrar graphics, const Graphics upgrade)
{
    static Point zero;
    static Unit none;
    Unit temp = none;
    Unit_Preserve(&temp, unit);
    // POSITION IS MAINTAINED.
    *unit = Unit_Make(zero, zero, grid, upgrade, unit->color, graphics, false, false, TRIGGER_NONE, false);
    Unit_Preserve(unit, &temp);
}

bool Unit_IsVillager(Unit* const unit)
{
    return unit->trait.type == TYPE_VILLAGER_FEMALE
        || unit->trait.type == TYPE_VILLAGER_MALE;
}
