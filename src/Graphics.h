#pragma once

#include "File.h"
#include "Type.h"

#include <stdbool.h>

typedef enum
{
#define FILE_X(name, file, prio, walkable, type, speed) name = file,
    FILE_X_GRAPHICS
#undef FILE_X
}
Graphics;

const char* Graphics_GetString(const Graphics);

uint8_t Graphics_GetHeight(const Graphics);

bool Graphics_IsWalkable(const Graphics);

Type Graphics_GetType(const Graphics);

int32_t Graphics_GetSpeed(const Graphics);
