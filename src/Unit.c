#include "Unit.h"

#include "Rect.h"
#include "Util.h"
#include "Log.h"
#include "Config.h"

#define MOCK_PATH_POINTS (2)

static void ConditionallySkipFirstPoint(Unit* const unit)
{
    if(unit->path.count > 1
    && unit->path_index == 0)
        unit->path_index++;
}

static Point GetDelta(Unit* const unit, const Grid grid)
{
    static Point zero;
    const Point point = (unit->path_index == unit->path.count - 1) ? unit->cart_grid_offset_goal : zero;
    const Point goal_grid_coords = Grid_GetGridPointWithOffset(grid, unit->path.point[unit->path_index], point);
    const Point unit_grid_coords = Grid_GetGridPointWithOffset(grid, unit->cart, unit->cart_grid_offset);
    return Point_Sub(goal_grid_coords, unit_grid_coords);
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
    unit->velocity = Point_Normalize(delta, unit->trait.max_speed);
    if(unit->is_chasing)
        Unit_SetDir(unit, Point_Sub(unit->interest->cell, unit->cell));
    else
    {
        const bool align = Point_Mag(unit->group_alignment) > CONFIG_UNIT_ALIGNMENT_DEADZONE;
        Unit_SetDir(unit, align ? unit->group_alignment : unit->velocity);
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

static void Decelerate(Unit* const unit)
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
    else Decelerate(unit);
}

static void CapSpeed(Unit* const unit)
{
    if(Point_Mag(unit->velocity) > unit->trait.max_speed)
        unit->velocity = Point_Normalize(unit->velocity, unit->trait.max_speed);
}

static void UpdateCart(Unit* const unit, const Grid grid)
{
    unit->cart_grid_offset = Grid_CellToOffset(grid, unit->cell);
    unit->cart = Grid_CellToCart(grid, unit->cell);
}

void Unit_UndoMove(Unit* const unit, const Grid grid)
{
    unit->cell = unit->cell_last;
    UpdateCart(unit, grid);
    static Point zero;
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
    if(unit->was_wall_pushed)
        return;
    if(unit->is_state_locked)
        return;
    const Graphics file = GetFileFromState(unit, state);
    unit->state = state;
    unit->file = file;
    if(reset_state_timer)
        unit->state_timer = 0;
}

static int32_t GetFramesFromState(Unit* const unit, const Registrar graphics, const State state)
{
    const Graphics file = GetFileFromState(unit, state);
    const Animation animation = graphics.animation[unit->color][file];
    return Animation_GetFramesPerDirection(animation);
}

static int32_t GetExpireFrames(Unit* const unit, const Registrar graphics)
{
    const Animation animation = graphics.animation[unit->color][unit->file];
    return animation.count;
}

Unit Unit_Make(const Point cart, const Grid grid, const Graphics file, const Color color, const Registrar graphics)
{
    static Unit zero;
    Unit unit = zero;
    unit.trait = Trait_Build(file);
    unit.file = file;
    static int32_t id;
    unit.id = id++;
    unit.shadow_id = -1;
    unit.cart = cart;
    unit.cell = Grid_CartToCell(grid, cart);
    unit.color = color;
    unit.state = STATE_IDLE;
    unit.health = unit.trait.max_health;
    if(unit.trait.can_expire)
        unit.expire_frames = GetExpireFrames(&unit, graphics);
    if(unit.trait.is_multi_state)
    {
        unit.attack_frames_per_dir = GetFramesFromState(&unit, graphics, STATE_ATTACK);
        unit.fall_frames_per_dir = GetFramesFromState(&unit, graphics, STATE_FALL);
        unit.decay_frames_per_dir = GetFramesFromState(&unit, graphics, STATE_DECAY);
    }
    Unit_UpdateEntropy(&unit);
    unit.entropy_static = Util_Rand();
    return unit;
}

void Unit_Print(Unit* const unit)
{
    Log_Append("type                  :: %d",    unit->trait.type);
    Log_Append("cart                  :: %d %d", unit->cart.x, unit->cart.y);
    Log_Append("cart_grid_offset      :: %d %d", unit->cart_grid_offset.x, unit->cart_grid_offset.y);
    Log_Append("cart_grid_offset_goal :: %d %d", unit->cart_grid_offset_goal.x, unit->cart_grid_offset_goal.y);
    Log_Append("cell                  :: %d %d", unit->cell.x, unit->cell.y);
    Log_Append("max_speed             :: %d",    unit->trait.max_speed);
    Log_Append("velocity              :: %d %d", unit->velocity.x, unit->velocity.y);
    Log_Append("path_index_timer      :: %d",    unit->path_index_timer);
    Log_Append("path_index            :: %d",    unit->path_index);
    Log_Append("path.count            :: %d",    unit->path.count);
    Log_Append("selected              :: %d",    unit->is_selected);
    Log_Append("file                  :: %d",    unit->file);
    Log_Append("file_name             :: %s",    unit->trait.file_name);
    Log_Append("id                    :: %d",    unit->id);
    Log_Append("shadow_id             :: %d",    unit->shadow_id);
    Log_Append("command_group         :: %d",    unit->command_group);
    Log_Append("health                :: %d",    unit->health);
    Log_Append("attack_frames_per_dir :: %d",    unit->attack_frames_per_dir);
    Log_Append("fall_frames_per_dir   :: %d",    unit->fall_frames_per_dir);
    Log_Append("decay_frames_per_dir  :: %d",    unit->decay_frames_per_dir);
    Log_Append("can_expire            :: %d",    unit->trait.can_expire);
    Log_Append("expire_frames         :: %d",    unit->expire_frames);
    Log_Append("state_timer           :: %d",    unit->state_timer);
    Log_Append("must_garbage_collect  :: %d",    unit->must_garbage_collect);
    Log_Append("");
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

bool Unit_InPlatoon(Unit* const unit, Unit* const other) // XXX. NEEDS check for same unit type as well.
{
    return unit->command_group == other->command_group
        && unit->color == other->color;
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
        unit->cart_grid_offset_goal = cart_grid_offset_goal;
        unit->path = Field_PathGreedyBest(field, unit->cart, cart_goal);
    }
}

void Unit_MockPath(Unit* const unit, const Point cart_goal, const Point cart_grid_offset_goal)
{
    if(!Unit_IsExempt(unit))
    {
        Unit_FreePath(unit);
        unit->cart_grid_offset_goal = cart_grid_offset_goal;
        unit->path = Points_New(MOCK_PATH_POINTS);
        unit->path = Points_Append(unit->path, unit->cart);
        unit->path = Points_Append(unit->path, cart_goal);
    }
}

int32_t Unit_Kill(Unit* const unit)
{
    Unit_Unlock(unit); // XXX. IS THIS NEEDED?
    unit->health = 0;
    if(unit->trait.is_building || unit->trait.type == TYPE_SHADOW)
    {
        unit->must_garbage_collect = true;
        if(unit->has_shadow)
            return unit->shadow_id;
    }
    else Unit_SetState(unit, STATE_FALL, true);
    return -1;
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

static bool ShouldEngage(Unit* const unit, const Grid grid)
{
    const Point diff = Point_Sub(unit->interest->cell, unit->cell);
    const int32_t reach = unit->trait.width + CONFIG_UNIT_SWORD_LENGTH;
    if(unit->interest->trait.is_building)
    {
        const Point feeler = Point_Normalize(diff, reach);
        const Point cell = Point_Add(unit->cell, feeler);
        const Point cart = Grid_CellToCart(grid, cell);
        const Point a = unit->interest->cart;
        const Point b = Point_Add(a, unit->interest->trait.dimensions);
        const Rect rect = { a, b };
        return Rect_ContainsPoint(rect, cart);
    }
    else return Point_Mag(diff) < reach;
}

void Unit_Melee(Unit* const unit, const Grid grid)
{
    if(unit->interest != NULL
    && !Unit_IsExempt(unit)
    && !Unit_IsExempt(unit->interest))
    {
        if(ShouldEngage(unit, grid))
        {
            Unit_SetState(unit, STATE_ATTACK, true);
            Unit_Lock(unit);
            if(unit->state_timer == Unit_GetLastAttackTick(unit))
            {
                unit->interest->health -= unit->trait.attack;
                Unit_Unlock(unit);
            }
            static Point zero;
            unit->velocity = zero;
        }
    }
    else Unit_Unlock(unit);
}

void Unit_Repath(Unit* const unit, const Field field)
{
    if(!Unit_IsExempt(unit)
    && unit->path_index_timer > CONFIG_UNIT_PATHING_TIMEOUT_CYCLES
    && unit->path.count > 0)
    {
        const Point cart_goal = unit->path.point[unit->path.count - 1];
        if(unit->path.count > MOCK_PATH_POINTS)
            Unit_FindPath(unit, cart_goal, unit->cart_grid_offset_goal, field);
        else
            Unit_MockPath(unit, cart_goal, unit->cart_grid_offset_goal);
    }
}

static Point Nudge(Unit* const unit)
{
    const uint32_t mag = UINT16_MAX;
    const Point half = { mag / 2, mag / 2 };
    return Point_Mul(Point_Sub(unit->entropy, half), mag);
}

Point Unit_Separate(Unit* const unit, Unit* const other)
{
    static Point zero;
    if(!Unit_IsExempt(other)
    && unit->id != other->id)
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

bool Unit_IsExempt(Unit* const unit)
{
    return State_IsDead(unit->state)
        || unit->trait.can_expire
        || unit->trait.type == TYPE_NONE
        || unit->trait.type == TYPE_RUBBLE
        || unit->trait.type == TYPE_SHADOW;
}

Point Unit_GetShift(Unit* const unit, const Point cart)
{
    const Point shift = { 0, 1 };
    const Point half = Point_Div(unit->trait.dimensions, 2);
    return Point_Add(cart, Point_Add(shift, half));
}

void Unit_UpdateEntropy(Unit* const unit)
{
    unit->entropy = Point_Rand();
}
