#pragma once

#include "Unit.h"
#include "Overview.h"
#include "Grid.h"
#include "Grid.h"
#include "Motive.h"
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
    Motive motive;
    int32_t food;
    int32_t gold;
    int32_t stone;
    int32_t wood;
    int32_t population;
}
Units;

Units Units_New(const Grid, const int32_t cpu_count, const int32_t max);

void Units_Free(const Units);

Stack Units_GetStackCart(const Units, const Point);

Field Units_Field(const Units, const Map);

void Units_ResetTiled(const Units);

Units Units_GenerateTestZone(Units, const Map, const Grid, const Registrar);

Units Units_Spawn(Units, const Point, const Point offset, const Grid, const Graphics, const Color, const Registrar, const Map, const bool is_floating);

Units Units_SpawnWithChild(Units, const Point, const Grid, const Graphics, const Color, const Registrar, const Graphics, const Map, const bool is_floating);

Units Units_SpawnTownCenter(Units units, const Point, const Grid, const Color, const Registrar, const Map, const bool is_floating);

void Units_ManageStacks(const Units);

bool Units_CanBuild(const Units, const Map, Unit* const);

Units Units_Service(Units, const Registrar, const Overview, const Grid, const Map, const Field);

Units Units_Caretake(Units, const Registrar, const Overview, const Grid, const Map, const Field);

void Units_StackStacks(const Units);

void Units_ResetStacks(const Units);

Units Units_Float(Units, const Registrar, const Overview, const Grid, const Map, const Motive);
