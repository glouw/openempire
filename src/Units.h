#pragma once

#include "Unit.h"
#include "Overview.h"
#include "Map.h"
#include "Registrar.h"
#include "Stack.h"

// Units are arranged in a linear array, but also referenced with
// a 2D tile array for quick point lookups.
//
// Given multiple units occupy the same tile, a unit stack
// can reference multiple units per tile.

typedef struct
{
    Unit* unit;
    int32_t count;
    int32_t max;

    Stack* stack;
    int32_t rows;
    int32_t cols;
}
Units;

Units Units_New(const int32_t max, const int32_t rows, const int32_t cols);

Units Units_Append(Units, const Unit);

void Units_Free(const Units);

Stack Units_GetStackCart(const Units, const Point);

void Units_Caretake(const Units, const Registrar, const Overview, const Grid, const Input, const Map);

bool Units_CanWalk(const Units, const Map, const Point);
