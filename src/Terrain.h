#pragma once

#include "File.h"

#include <stdbool.h>

typedef enum
{
#define FILE_X(name, file, prio, walkable, type, max_speed, accel, health) name = file,
    FILE_X_TERRAIN
#undef FILE_X
}
Terrain;

uint8_t Terrain_GetHeight(const Terrain);

const char* Terrain_GetString(const Terrain);

bool Terrain_IsWalkable(const Terrain);
