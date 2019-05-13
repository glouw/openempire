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
    Point point;
    SDL_Surface* surface;
    bool needs_clipping;
    uint8_t height;
}
Tile;

Tile Tile_GetTerrain(const Overview, const Point, const Animation, const Terrain);

Tile Tile_GetGraphics(const Overview, const Point, const Animation, const Graphics);
