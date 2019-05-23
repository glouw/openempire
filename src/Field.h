#pragma once

#include "Points.h"
#include "Map.h"

#include <stdint.h>

typedef struct
{
    char* object;
    int32_t rows;
    int32_t cols;
}
Field;

Points Field_SearchBreadthFirst(const Field, const Point start, const Point goal);

Field Field_New(const Map);

void Field_Free(const Field);
