#pragma once

#include "Point.h"
#include "Overview.h"
#include "Lines.h"
#include "Registrar.h"
#include "Points.h"
#include "Terrain.h"

#define MAP_HEIGHT_MAX           (255)
#define MAP_HEIGHT_TREES         (220)
#define MAP_HEIGHT_GRASS         (140)
#define MAP_HEIGHT_DIRT          (125)
#define MAP_HEIGHT_WATER_SHALLOW (100)
#define MAP_HEIGHT_WATER_NORMAL   (75)
#define MAP_HEIGHT_WATER_DEEP     (50)

typedef struct
{
    Terrain* file;
    int32_t* height;
    int32_t size;
    int32_t tile_width;
    int32_t tile_height;
    Color color[TERRAIN_COUNT];
}
Map;

Map Map_Make(const int32_t power, const Registrar terrain);

Terrain Map_GetTerrainFile(const Map, const Point);

void Map_SetTerrainFile(const Map, const Point, const Terrain);

void Map_Free(const Map);

Points Map_GetBlendBox(const Map, const Point inner);

Lines Map_GetBlendLines(const Map, const Points);

int32_t Map_GetHeight(const Map, const Point);
