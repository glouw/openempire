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

Tile Tiles_SelectOne(const Tiles, const Point);

void Tiles_SelectSimilar(const Tiles, const Tile);

void Tiles_SelectWithBox(const Tiles, const Rect);
