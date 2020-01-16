#pragma once

#include "Point.h"
#include "Overview.h"
#include "Lines.h"
#include "Registrar.h"
#include "Points.h"
#include "Terrain.h"

typedef struct
{
    Terrain* file;
    int32_t* height;
    int32_t size;
    int32_t tile_width;
    int32_t tile_height;
}
Map;

Map Map_Make(const int32_t power, const Registrar terrain);

Terrain Map_GetTerrainFile(const Map, const Point);

void Map_SetTerrainFile(const Map, const Point, const Terrain file);

void Map_Free(const Map);

Points Map_GetBlendBox(const Map, const Point inner);

Lines Map_GetBlendLines(const Map, const Points);
