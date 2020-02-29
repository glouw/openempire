#include "Unit.h"

#include "Rect.h"
#include "Util.h"
#include "Resource.h"
#include "Config.h"

#define MOCK_PATH_POINTS (2)

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
    if(unit->is_engaged)
    {
        const Point cell = unit->interest->trait.is_inanimate
            ? unit->interest->cell_inanimate
            : unit->interest->cell;
        const Point dir = Point_Sub(cell, unit->cell);
        Unit_SetDir(unit, dir);
    }
    else
    {
        const bool align = Point_Mag(unit->group_alignment) > CONFIG_UNIT_ALIGNMENT_DEADZONE;
        const Point dir = align ? unit->group_alignment : unit->velocity;
        Unit_SetDir(unit, dir);
    }
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
    Unit_SetState(unit, STATE_MOVE, false);
    if(Point_Mag(unit->velocity) < CONFIG_UNIT_VELOCITY_DEADZONE)
        Unit_SetState(unit, STATE_IDLE, false);
}

static Graphics GetFileFromState(Unit* const unit, const State state)
{
    const int32_t base = (int32_t) unit->file - (int32_t) unit->state;
    const int32_t next = base + (int32_t) state;
    return (Graphics) next;
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
        const Graphics file = GetFileFromState(unit, state);
        unit->state = state;
        unit->file = file;
        if(reset_state_timer)
            unit->state_timer = 0;
    }
}

static int32_t GetFramesFromState(Unit* const unit, const Registrar graphics, const State state)
{
    const Graphics file = GetFileFromState(unit, state);
    const Animation animation = graphics.animation[unit->color][file];
    return Animation_GetFramesPerDirection(animation);
}

static int32_t GetExpireFrames(Unit* const unit, const Registrar graphics)
{
    return graphics.animation[unit->color][unit->file].count;
}

Unit Unit_Make(Point cart, const Point offset, const Grid grid, const Graphics file, const Color color, const Registrar graphics, const bool at_center, const bool is_floating, const Trigger trigger)
{
    static int32_t id;
    static Unit zero;
    Unit unit = zero;
    unit.trait = Trait_Build(file);
    unit.file = file;
    unit.id = id;
    if(!is_floating)
        id += 1;
    unit.parent_id = -1;
    unit.color = color;
    unit.state = STATE_IDLE;
    unit.health = unit.trait.max_health;
    unit.is_floating = is_floating;
    unit.trigger = trigger;
    if(!is_floating)
    {
        unit.entropy = Point_Rand();
        unit.entropy_static = Util_Rand();
    }
    if(at_center)
    {
        const Point center = Point_Div(unit.trait.dimensions, 2);
        cart = Point_Sub(cart, center);
    }
    unit.cell = Grid_CartToCell(grid, cart);
    if(Point_IsEven(unit.trait.dimensions))
    {
        const Point shift = {
            grid.tile_cart_mid.x * CONFIG_GRID_CELL_SIZE,
            grid.tile_cart_mid.y * CONFIG_GRID_CELL_SIZE,
        };
        unit.cell = Point_Sub(unit.cell, shift);
    }
    const Point mid = { grid.tile_cart_mid.x, -grid.tile_cart_mid.y };
    unit.cell = unit.trait.needs_midding
        ? Point_Add(unit.cell, Grid_OffsetToCell(mid))
        : Point_Add(unit.cell, Grid_OffsetToCell(offset));
    UpdateCart(&unit, grid);
    if(unit.trait.can_expire)
        unit.expire_frames = GetExpireFrames(&unit, graphics);
    if(unit.trait.is_multi_state)
    {
        unit.attack_frames_per_dir = GetFramesFromState(&unit, graphics, STATE_ATTACK);
        unit.fall_frames_per_dir   = GetFramesFromState(&unit, graphics, STATE_FALL);
        unit.decay_frames_per_dir  = GetFramesFromState(&unit, graphics, STATE_DECAY);
    }
    if(unit.trait.type == TYPE_FIRE
    || unit.trait.type == TYPE_RUBBLE)
        unit.is_timing_to_collect = true;
    return unit;
}

void Unit_Print(Unit* const unit)
{
    printf("action                :: %d\n",    unit->trait.action);
    printf("type                  :: %d\n",    unit->trait.type);
    printf("cart                  :: %d %d\n", unit->cart.x, unit->cart.y);
    printf("cart_grid_offset      :: %d %d\n", unit->cart_grid_offset.x, unit->cart_grid_offset.y);
    printf("cart_grid_offset_goal :: %d %d\n", unit->cart_grid_offset_goal.x, unit->cart_grid_offset_goal.y);
    printf("cell                  :: %d %d\n", unit->cell.x, unit->cell.y);
    printf("max_speed             :: %d\n",    unit->trait.max_speed);
    printf("velocity              :: %d %d\n", unit->velocity.x, unit->velocity.y);
    printf("path_index_timer      :: %d\n",    unit->path_index_timer);
    printf("path_index            :: %d\n",    unit->path_index);
    printf("path.count            :: %d\n",    unit->path.count);
    printf("selected              :: %d\n",    unit->is_selected);
    printf("file                  :: %d\n",    unit->file);
    printf("file_name             :: %s\n",    unit->trait.file_name);
    printf("id                    :: %d\n",    unit->id);
    printf("parent_id             :: %d\n",    unit->parent_id);
    printf("command_group         :: %d\n",    unit->command_group);
    printf("health                :: %d\n",    unit->health);
    printf("attack_frames_per_dir :: %d\n",    unit->attack_frames_per_dir);
    printf("fall_frames_per_dir   :: %d\n",    unit->fall_frames_per_dir);
    printf("decay_frames_per_dir  :: %d\n",    unit->decay_frames_per_dir);
    printf("can_expire            :: %d\n",    unit->trait.can_expire);
    printf("expire_frames         :: %d\n",    unit->expire_frames);
    printf("state_timer           :: %d\n",    unit->state_timer);
    printf("must_garbage_collect  :: %d\n",    unit->must_garbage_collect);
    printf("\n");
}

void ApplyStressors(Unit* const unit)
{
    unit->velocity = Point_Add(unit->velocity, unit->stressors);
}

void Unit_Flow(Unit* const unit, const Grid grid)
{
    FollowPath(unit, grid);
    ApplyStressors(unit);
    CapSpeed(unit);
}

bool Unit_InPlatoon(Unit* const unit, Unit* const other)
{
    return unit->command_group == other->command_group && unit->color == other->color;
}

void Unit_SetDir(Unit* const unit, const Point dir)
{
    if(unit->dir_timer > CONFIG_UNIT_DIRECTION_TIMER_EXPIRE)
    {
        unit->dir = Direction_CartToIso(Direction_GetCart(dir));
        unit->dir_timer = 0;
    }
}

void Unit_FindPath(Unit* const unit, const Point cart_goal, const Point cart_grid_offset_goal, const Field field)
{
    if(!Unit_IsExempt(unit))
    {
        Unit_FreePath(unit);
        unit->cart_goal = cart_goal;
        unit->cart_grid_offset_goal = cart_grid_offset_goal;
        unit->path = Field_PathAStar(field, unit->cart, cart_goal);
    }
}

void Unit_MockPath(Unit* const unit, const Point cart_goal, const Point cart_grid_offset_goal)
{
    if(!Unit_IsExempt(unit))
    {
        Unit_FreePath(unit);
        unit->cart_goal = cart_goal;
        unit->cart_grid_offset_goal = cart_grid_offset_goal;
        unit->path = Points_New(MOCK_PATH_POINTS);
        unit->path = Points_Append(unit->path, unit->cart);
        unit->path = Points_Append(unit->path, cart_goal);
    }
}

void Unit_Kill(Unit* const unit)
{
    unit->health = 0;
    unit->is_selected = false;
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

static bool MustEngage(Unit* const unit, const Grid grid)
{
    const Point diff = Point_Sub(
            unit->interest->trait.is_inanimate
                ? unit->interest->cell_inanimate
                : unit->interest->cell,
            unit->cell);
    const int32_t width = UTIL_MAX(unit->trait.width, unit->interest->trait.width);
    int32_t reach = CONFIG_UNIT_SWORD_LENGTH + width;
    if(unit->interest->trait.is_inanimate)
    {
        reach -= width / 2;
        const Point feeler = Point_Normalize(diff, reach);
        const Point cell = Point_Add(unit->cell, feeler);
        const Point cart = Grid_CellToCart(grid, cell);
        const Point a = unit->interest->cart;
        const Point b = Point_Add(a, unit->interest->trait.dimensions);
        const Rect rect = { a, b };
        return Rect_ContainsPoint(rect, cart);
    }
    else
        return Point_Mag(diff) < reach;
}

static Resource CollectResource(Unit* const unit)
{
    Resource resource = {
        TYPE_NONE,
        unit->trait.attack
    };
    switch(unit->interest->trait.type)
    {
    case TYPE_TREE:
        resource.type = TYPE_WOOD;
        break;
    case TYPE_STONE_MINE:
        resource.type = TYPE_STONE;
        break;
    case TYPE_GOLD_MINE:
        resource.type = TYPE_GOLD;
        break;
    case TYPE_BERRY_BUSH:
        resource.type = TYPE_FOOD;
        break;
    default:
        break;
    }
    return resource;
}

static bool MustDisengage(Unit* const unit)
{
    return unit->state == STATE_ATTACK
        && unit->state_timer >= Unit_GetLastAttackTick(unit)
        && !Unit_IsDead(unit->interest);
}

Resource Unit_Melee(Unit* const unit, const Grid grid)
{
    if(unit->interest != NULL
    && !Unit_IsExempt(unit)
    && !Unit_IsExempt(unit->interest))
    {
        if(MustEngage(unit, grid))
        {
            Unit_SetState(unit, STATE_ATTACK, true);
            Unit_Lock(unit);
        }
        if(MustDisengage(unit))
        {
            unit->interest->health -= unit->trait.attack;
            Unit_Unlock(unit);
            if(unit->trait.type == TYPE_VILLAGER)
                return CollectResource(unit);
        }
    }
    else
        Unit_Unlock(unit);
    const Resource none = { TYPE_NONE, 0 };
    return none;
}

void Unit_Repath(Unit* const unit, const Field field)
{
    if(!Unit_IsExempt(unit)
    && unit->path_index_timer > CONFIG_UNIT_PATHING_TIMEOUT_CYCLES
    && unit->path.count > 0)
        (unit->path.count > MOCK_PATH_POINTS)
            ? Unit_FindPath(unit, unit->cart_goal, unit->cart_grid_offset_goal, field)
            : Unit_MockPath(unit, unit->cart_goal, unit->cart_grid_offset_goal);
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
    return State_IsDead(unit->state) || unit->trait.is_detail || unit->must_garbage_collect;
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

bool Unit_IsTriggerValid(Unit* const flag)
{
    return !flag->is_triggered && flag->trigger != TRIGGER_NONE;
}

void Unit_Preserve(Unit* const to, const Unit* const from)
{
    to->id = from->id;
    to->parent_id = from->parent_id;
    to->has_children = from->has_children;
    to->path = from->path;
    to->dir = from->dir;
    to->health = from->health;
}
