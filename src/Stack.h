#pragma once

#include "Unit.h"

// UNITS, WHEN ALLIGNED IN A GRID, MUST BE REFERENCED
// TO THEIR TRUE LINEAR UNIT ARRAY (DEFINED IN UNITS.[CH]).
// SOMETIMES, MORE THAN ONE UNIT WILL OCCUPY A SINGLE GRID LOCATION,
// AND SO THE UNIT STACK CAN REFERENCE ANY NUMBER OF UNITS ON A SINGLE TILE.

typedef struct
{
    Unit** reference;
    int32_t count;
    int32_t max;
}
Stack;

Stack Stack_Build(const int32_t max);

void Stack_Append(Stack* const, Unit* const);

void Stack_Free(const Stack);

bool Stack_IsWalkable(const Stack);

int32_t Stack_GetMaxPathIndex(const Stack, Unit* const);
