#pragma once

#include "Tile.h"
#include "Rect.h"
#include "Units.h"

typedef struct
{
    Tile* tile;
    int32_t count;
}
Tiles;

Tiles Tiles_PrepGraphics(const Registrar, const Overview, const Units, const Points);

Tiles Tiles_PrepTerrain(const Registrar, const Map, const Overview, const Points);

void Tiles_Free(const Tiles);

void Tiles_SelectOne(const Tiles, const Point);

void Tiles_SelectMany(const Tiles, const Rect);
