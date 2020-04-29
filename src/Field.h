#pragma once

#include "Points.h"
#include "Map.h"

#include <stdint.h>

#define FIELD_WALKABLE_SPACE (' ')
#define FIELD_OBSTRUCT_SPACE ('#')

typedef struct
{
    char* object;
    int32_t size;
    int32_t area;
}
Field;

Points Field_PathAStar(const Field, const Point start, const Point goal);

char Field_Get(const Field, const Point);

void Field_Set(const Field, const Point, const char ch);

bool Field_IsWalkable(const Field, const Point);

Field Field_Make(const int32_t size);

void Field_Clear(const Field);

void Field_Free(const Field);
