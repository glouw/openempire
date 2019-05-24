#pragma once

#include "Frame.h"
#include "Point.h"
#include "File.h"
#include "Map.h"
#include "Overview.h"
#include "Animation.h"
#include "Unit.h"
#include "Graphics.h"

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct
{
    Frame frame;
    Point iso_point_global;
    Point iso_fractional_local;
    SDL_Surface* surface;
    bool needs_clipping;
    uint8_t height;
    Unit* reference;
}
Tile;

bool Tile_ContainsPoint(const Tile, const Point);

Tile Tile_GetTerrain(const Overview, const Point point, const Animation, const Terrain);

Tile Tile_GetGraphics(const Overview, const Point point, const Point fractional, const Animation, const Graphics);

Point Tile_GetScreenCoords(const Tile, const int32_t x, const int32_t y);

Tile Tile_Construct(const Overview, const Point cart_point, const Point cart_fractional, const Animation, const int32_t index);

Point Tile_GetHotSpotCoords(const Tile);
