#pragma once

#include "Points.h"
#include "Graphics.h"
#include "State.h"
#include "Color.h"
#include "Grid.h"
#include "Direction.h"

typedef struct
{
    Point cart;
    Point cart_grid_offset;
    Point cart_grid_offset_goal;
    Point cell;
    Point cell_last;
    int32_t max_speed;
    int32_t accel;
    Point velocity;
    Points path;
    int32_t path_index;
    int32_t path_index_time;
    bool selected;
    Graphics file;
    const char* file_name;
    int32_t id;
    int32_t command_group;
    Color color;
    Direction dir;
    Point stressors;
    State state;
    int32_t health;
    int32_t attack;
    int32_t timer;
    int32_t width;
    Type type;
    Point alignment;
}
Unit;

Unit Unit_Make(const Point cart, const Grid, const Graphics file, const Color);

void Unit_UndoMove(Unit* const, const Grid);

void Unit_Move(Unit* const, const Grid);

void Unit_Print(Unit* const);

void Unit_Flow(Unit* const, const Grid);

bool Unit_InPlatoon(Unit* const unit, Unit* const other);

void Unit_UpdateFileByState(Unit* const, const State, const bool reset_timer);

void Unit_FreePath(Unit* const);

void Unit_SetDir(Unit* const, const Point);
