#include "Unit.h"

#include "Rect.h"
#include "Util.h"
#include "Config.h"

static void ConditionallySkipFirstPoint(Unit* const unit)
{
    if(unit->path.count > 1 && unit->path_index == 0)
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

void Unit_UpdatePathIndex(Unit* const unit, const int32_t index)
{
    unit->path_index = index;
    unit->path_index_timer = 0;
}

void Unit_FreePath(Unit* const unit)
{
    Unit_UpdatePathIndex(unit, 0);
    unit->path = Points_Free(unit->path);
}

static void ReachGoal(Unit* const unit)
{
    Unit_UpdatePathIndex(unit, unit->path_index + 1);
    if(unit->path_index >= unit->path.count)
        Unit_FreePath(unit);
}

static void GotoGoal(Unit* const unit, const Point delta)
{
    const Point accel = Point_Normalize(delta, unit->accel);
    unit->velocity = Point_Add(unit->velocity, accel);
    if(unit->is_chasing)
        Unit_SetDir(unit, Point_Sub(unit->cell_of_interest, unit->cell));
    else
    {
        const bool align = Point_Mag(unit->group_alignment) > CONFIG_UNIT_ALIGNMENT_DEADZONE;
        Unit_SetDir(unit, align ? unit->group_alignment : accel);
    }
}

static void AccelerateAlongPath(Unit* const unit, const Grid grid)
{
    const Point delta = GetDelta(unit, grid);
    if(Point_Mag(delta) < CONFIG_POINT_GOAL_CLOSE_ENOUGH_MAG)
        ReachGoal(unit);
    else
        GotoGoal(unit, delta);
}

static void Decelerate(Unit* const unit)
{
    static Point zero;
    const Point deccel = Point_Normalize(unit->velocity, unit->accel);
    const Point velocity = Point_Sub(unit->velocity, deccel);
    const Point dot = Point_Dot(velocity, unit->velocity);
    unit->velocity = (dot.x >= 0 && dot.y >= 0) ? velocity : zero; // XXX. Double check the math...
}

static void FollowPath(Unit* const unit, const Grid grid)
{
    if(unit->path.count > 0)
    {
        ConditionallySkipFirstPoint(unit);
        AccelerateAlongPath(unit, grid);
    }
    else if(Point_Mag(unit->velocity) > 0) Decelerate(unit);
}

static void CapSpeed(Unit* const unit)
{
    if(Point_Mag(unit->velocity) > unit->max_speed)
        unit->velocity = Point_Normalize(unit->velocity, unit->max_speed);
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
    Unit_UpdateFileByState(unit, STATE_MOVE, false);
    if(Point_Mag(unit->velocity) < CONFIG_UNIT_VELOCITY_DEADZONE)
        Unit_UpdateFileByState(unit, STATE_IDLE, false);
}

static Graphics GetFileFromState(Unit* const unit, const State state)
{
    const int32_t base = (int32_t) unit->file - (int32_t) unit->state;
    const int32_t next = base + (int32_t) state;
    const Graphics file = (Graphics) next;
    return file;
}

void Unit_UpdateFileByState(Unit* const unit, const State state, const bool reset_timer)
{
    const Graphics file = GetFileFromState(unit, state);
    unit->state = state;
    unit->file = file;
    if(reset_timer)
        unit->timer = 0;
}

static int32_t GetFramesFromState(Unit* const unit, const State state, const Registrar graphics)
{
    const Graphics attack = GetFileFromState(unit, state);
    const Animation animation = graphics.animation[unit->color][attack];
    const int32_t frames = Animation_GetFramesPerDirection(animation);
    return frames;
}

Unit Unit_Make(const Point cart, const Grid grid, const Graphics file, const Color color, const Registrar graphics)
{
    static Unit zero;
    Unit unit = zero;
    unit.cart = cart;
    unit.cell = Grid_CartToCell(grid, cart);
    unit.color = color;
    unit.state = STATE_IDLE; // This is the base state, and is required for all new units as UpdateFileByState() references this base state.
    unit.max_speed = Graphics_GetMaxSpeed(file);
    unit.accel = Graphics_GetAcceleration(file);
    unit.file_name = Graphics_GetString(file);
    unit.max_health = Graphics_GetHealth(file);
    unit.health = unit.max_health;
    unit.attack = Graphics_GetAttack(file);
    unit.file = file;
    unit.timer = Util_Rand() % 10;
    unit.width = Graphics_GetWidth(file);
    unit.type = Graphics_GetType(file);
    unit.attack_frames_per_dir = GetFramesFromState(&unit, STATE_ATTACK, graphics);
    unit.fall_frames_per_dir = GetFramesFromState(&unit, STATE_FALL, graphics);
    return unit;
}

void Unit_Print(Unit* const unit)
{
    Util_Log("cart                  :: %d %d\n", unit->cart.x, unit->cart.y);
    Util_Log("cart_grid_offset      :: %d %d\n", unit->cart_grid_offset.x, unit->cart_grid_offset.y);
    Util_Log("cart_grid_offset_goal :: %d %d\n", unit->cart_grid_offset_goal.x, unit->cart_grid_offset_goal.y);
    Util_Log("cell                  :: %d %d\n", unit->cell.x, unit->cell.y);
    Util_Log("max_speed             :: %d\n",    unit->max_speed);
    Util_Log("accel                 :: %d\n",    unit->accel);
    Util_Log("velocity              :: %d %d\n", unit->velocity.x, unit->velocity.y);
    Util_Log("path_index_timer      :: %d\n",    unit->path_index_timer);
    Util_Log("path_index            :: %d\n",    unit->path_index);
    Util_Log("path.count            :: %d\n",    unit->path.count);
    Util_Log("selected              :: %d\n",    unit->is_selected);
    Util_Log("file                  :: %d\n",    unit->file);
    Util_Log("file_name             :: %s\n",    unit->file_name);
    Util_Log("id                    :: %d\n",    unit->id);
    Util_Log("command_group         :: %d\n",    unit->command_group);
    Util_Log("health                :: %d\n",    unit->health);
    Util_Log("attack_frames_per_dir :: %d\n",    unit->attack_frames_per_dir);
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
    if(!State_IsDead(unit->state))
    {
        Unit_FreePath(unit);
        unit->cart_grid_offset_goal = cart_grid_offset_goal;
        unit->path = Field_PathGreedyBest(field, unit->cart, cart_goal);
    }
}

// A straight line path finder. Takes the unit from <cart_goal> to <cart_grid_offset_goal>. Field objects are totally ignored.
// Use with best discretion.

void Unit_MockPath(Unit* const unit, const Point cart_goal, const Point cart_grid_offset_goal)
{
    if(!State_IsDead(unit->state))
    {
        Unit_FreePath(unit);
        unit->cart_grid_offset_goal = cart_grid_offset_goal;
        unit->path = Points_New(2);
        unit->path = Points_Append(unit->path, unit->cart);
        unit->path = Points_Append(unit->path, cart_goal);
    }
}

void Unit_Kill(Unit* const unit)
{
    unit->health = 0;
    Unit_UpdateFileByState(unit, STATE_FALL, true);
}
