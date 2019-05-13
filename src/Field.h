#pragma once

#include "Points.h"

#include <stdint.h>

typedef struct
{
    char* object;
    int32_t rows;
    int32_t cols;
}
Field;

Points Field_SearchBreadthFirst(const Field, const Point start, const Point goal);
