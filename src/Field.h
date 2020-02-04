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
}
Field;

Points Field_PathAStar(const Field, const Point start, const Point goal);

void Field_Free(const Field);

char Field_Get(const Field, const Point);

void Field_Set(const Field, const Point, const char ch);
