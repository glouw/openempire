#pragma once

#include "Rect.h"

#include <stdint.h>

typedef struct
{
    Rect* rect;
    int32_t count;
}
Rects;

Rects Rects_Make(const int32_t count);

void Rects_Free(const Rects);
