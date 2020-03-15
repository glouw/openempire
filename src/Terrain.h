#pragma once

#include "File.h"

#include <stdbool.h>

typedef enum
{
#define FILE_X(name, file, upgrade, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, resource, dimensions, action, detail, midding) name = file,
    FILE_X_TERRAIN
#undef FILE_X
    TERRAIN_COUNT
}
Terrain;

uint8_t Terrain_GetHeight(const Terrain);

const char* Terrain_GetString(const Terrain);

bool Terrain_IsWalkable(const Terrain);
