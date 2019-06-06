#pragma once

#include "Unit.h"

// Units, when alligned in a grid, must be referenced
// to their true linear unit array (defined in Units.[ch]).
// Sometimes, more than one unit will occupy a single grid location,
// and so the unit stack can reference any number of units on a single tile.

typedef struct
{
    Unit** reference;
    int32_t count;
    int32_t max;
    Point center_of_mass;
}
Stack;

Stack Stack_Build(const int32_t max);

void Stack_Append(Stack* const, Unit* const);

void Stack_Free(const Stack);

void Stack_Sort(const Stack);

bool Stack_IsWalkable(const Stack);

void Stack_UpdateCenterOfMass(Stack* const);

int32_t Stack_GetMaxPathIndex(const Stack);
