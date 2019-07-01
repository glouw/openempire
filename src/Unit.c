#include "Unit.h"

#include "Rect.h"
#include "Util.h"

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

void Unit_FreePath(Unit* const unit)
{
    unit->path_index = 0;
    unit->path_index_time = 0;
    unit->path = Points_Free(unit->path);
}

static void ReachGoal(Unit* const unit)
{
    unit->path_index++;
    unit->path_index_time = 0;
    if(unit->path_index >= unit->path.count)
    {
        Unit_UpdateFileByState(unit, STATE_IDLE, false);
        Unit_FreePath(unit);
    }
}

static void GotoGoal(Unit* const unit, const Point delta)
{
    const Point dv = Point_Normalize(delta, unit->accel);
    unit->velocity = Point_Add(unit->velocity, dv);
    unit->dir = Direction_CartToIso(Direction_GetCart(dv));
}

static void AccelerateAlongPath(Unit* const unit, const Grid grid)
{
    const Point delta = GetDelta(unit, grid);
    unit->path_index_time++;
    if(Point_Mag(delta) < 10) // XXX: Is this too small? What about really fast moving guys?
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
}

void Unit_UpdateFileByState(Unit* const unit, const State state, const bool reset_timer)
{
    const int32_t base = (int32_t) unit->file - (int32_t) unit->state;
    const int32_t next = base + (int32_t) state;
    const Graphics file = (Graphics) next;
    unit->state = state;
    unit->file = file;
    if(reset_timer)
        unit->timer = 0;
}

Unit Unit_Make(const Point cart, const Grid grid, const Graphics file, const Color color)
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
    unit.health = Graphics_GetHealth(file);
    unit.attack = Graphics_GetAttack(file);
    unit.file = file;
    unit.timer = Util_Rand() % 10;
    unit.width = Graphics_GetWidth(file);
    return unit;
}

void Unit_Print(Unit* const unit)
{
    Util_Log("cart                  :: %ld %ld\n", unit->cart.x, unit->cart.y);
    Util_Log("cart_grid_offset      :: %ld %ld\n", unit->cart_grid_offset.x, unit->cart_grid_offset.y);
    Util_Log("cart_grid_offset_goal :: %ld %ld\n", unit->cart_grid_offset_goal.x, unit->cart_grid_offset_goal.y);
    Util_Log("cell                  :: %ld %ld\n", unit->cell.x, unit->cell.y);
    Util_Log("max_speed             :: %d\n",      unit->max_speed);
    Util_Log("accel                 :: %d\n",      unit->accel);
    Util_Log("velocity              :: %ld %ld\n", unit->velocity.x, unit->velocity.y);
    Util_Log("path_index_time       :: %d\n",      unit->path_index_time);
    Util_Log("path_index            :: %d\n",      unit->path_index);
    Util_Log("path.count            :: %d\n",      unit->path.count);
    Util_Log("selected              :: %d\n",      unit->selected);
    Util_Log("file                  :: %d\n",      unit->file);
    Util_Log("file_name             :: %s\n",      unit->file_name);
    Util_Log("id                    :: %d\n",      unit->id);
    Util_Log("group                 :: %d\n",      unit->command_group);
    Util_Log("health                :: %d\n",      unit->health);
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
