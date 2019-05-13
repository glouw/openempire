#pragma once

#include "File.h"

typedef enum
{
#define FILE_X(name, file, prio) name = file,
    FILE_X_TERRAIN
#undef FILE_X
}
Terrain;

uint8_t Terrain_GetHeight(const Terrain);

const char* Terrain_GetString(const Terrain);
