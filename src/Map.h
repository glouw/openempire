#pragma once

#include "Point.h"
#include "Overview.h"
#include "File.h"
#include "Registrar.h"
#include "Points.h"
#include "Terrain.h"

typedef struct
{
    Terrain* file;
    int32_t rows;
    int32_t cols;
    int32_t tile_width;
    int32_t tile_height;
}
Map;

Map Map_Make(const int32_t size, const Registrar terrain);

Terrain Map_GetTerrainFile(const Map, const Point);

void Map_SetTerrainFile(const Map, const Point, const Terrain file);

void Map_Free(const Map);

void Map_Edit(const Map, const Overview, const Input);

Points Map_GetBlendBox(const Map, const Point inner);
