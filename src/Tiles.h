#pragma once

#include "Tile.h"
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

void Tiles_Select(const Tiles, const Point click);
