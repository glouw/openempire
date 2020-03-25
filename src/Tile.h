#pragma once

#include "Frame.h"
#include "Point.h"
#include "Rect.h"
#include "Map.h"
#include "Overview.h"
#include "Animation.h"
#include "Unit.h"
#include "Direction.h"

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct
{
    Unit* reference;
    SDL_Surface* surface;
    Frame frame;
    Point iso_pixel;
    Point iso_pixel_offset;
    uint32_t height;
    bool needs_clipping;
    bool flip_vert;
    bool totally_offscreen;
    Rect bound;
    bool is_floating;
}
Tile;

bool Tile_ContainsPoint(const Tile, const Point);

Tile Tile_GetTerrain(const Overview, const Grid grid, const Point cart, const Animation, const Terrain);

Tile Tile_GetGraphics(const Overview, const Grid grid, const Point cart, const Point cart_grid_offset, const Animation, Unit* const reference);

Point Tile_GetTopLeftOffsetCoords(const Tile, const int32_t x, const int32_t y);

Point Tile_GetHotSpotCoords(const Tile);

Point Tile_GetTopLeftCoords(const Tile);

Rect Tile_GetFrameOutline(const Tile);

bool Tile_IsHotspotInRect(const Tile, const Rect);

void Tile_Select(const Tile, const Color);

Tile Tile_Clip(Tile tile, const Rect);
