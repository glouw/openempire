#pragma once

#include "Frame.h"
#include "Point.h"
#include "File.h"
#include "Map.h"
#include "Overview.h"
#include "Animation.h"
#include "Graphics.h"

#include <SDL2/SDL.h>

typedef struct
{
    Frame frame;
    Point iso_point;
    Point iso_fractional;
    SDL_Surface* surface;
    bool needs_clipping;
    uint8_t height;
}
Tile;

Tile Tile_GetTerrain(const Overview, const Point point, const Animation, const Terrain);

Tile Tile_GetGraphics(const Overview, const Point point, const Point fractional, const Animation, const Graphics);

Point Tile_GetScreenCoords(const Tile, const int32_t x, const int32_t y);
