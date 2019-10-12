#pragma once

#include "Unit.h"
#include "Overview.h"
#include "Window.h"
#include "Action.h"
#include "Field.h"
#include "Map.h"
#include "Points.h"
#include "Registrar.h"
#include "Stack.h"

// Units are arranged in a linear array, but also referenced with
// a 2D tile array for quick point lookups.
// Given multiple units occupy the same tile, a unit stack
// can reference multiple units per tile.
typedef struct
{
    Unit* unit;
    Stack* stack;
    int32_t count;
    int32_t max;
    int32_t rows;
    int32_t cols;
    int32_t command_group_next;
    int32_t select_count;
    int32_t cpu_count;
    int32_t repath_index;
    Action action;
}
Units;

Units Units_New(const Map map, const Grid grid, const Registrar graphics);

void Units_Free(const Units);

Stack Units_GetStackCart(const Units, const Point);

Units Units_Caretake(Units, const Registrar, const Overview, const Input, const Map, const Field, const Window);

Field Units_Field(const Units, const Map);

void Units_ResetTiled(const Units);

Units Units_GenerateTestZone(Units, const Map, const Grid, const Registrar);

Units Units_Spawn(Units, const Point, const Grid, const Graphics, const Color, const Registrar);

Units Units_SpawnWithShadow(Units, const Point, const Grid, const Graphics, const Color, const Registrar, const Graphics);

Units Units_SpawnWithOffset(Units, const Point, const Point, const Overview, const Graphics, const Color, const Registrar);

void Units_ManageStacks(const Units);
