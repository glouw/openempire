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

Tiles Tiles_PrepGraphics(const Registrar, const Overview, const Grid, const Units, const Points);

Tiles Tiles_PrepTerrain(const Registrar, const Map, const Overview, const Grid, const Points);

void Tiles_Free(const Tiles);

Tile Tiles_Get(const Tiles, const Point);

int32_t Tiles_SelectSimilar(const Tiles, const Tile, const Color);

int32_t Tiles_SelectWithBox(const Tiles, const Rect, const Color);

void Tiles_SortByHeight(const Tiles);

Tiles Tiles_Copy(const Tiles);
