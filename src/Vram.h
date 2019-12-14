#pragma once

#include "Point.h"
#include "Frame.h"
#include "Lines.h"
#include "Color.h"
#include "Registrar.h"
#include "Motive.h"
#include "Units.h"
#include "Rects.h"
#include "Map.h"
#include "Overview.h"
#include "Tiles.h"
#include "Blendomatic.h"

#include <stdint.h>
#include <SDL2/SDL.h>

typedef struct
{
    uint32_t* pixels;
    int32_t width;
    int32_t xres;
    int32_t yres;
    int32_t cpu_count;
    Rects channel_rects;
}
Vram;

Vram Vram_Lock(SDL_Texture* const, const int32_t xres, const int32_t yres, const int32_t cpu_count);

void Vram_Unlock(SDL_Texture* const);

void Vram_DrawCross(const Vram, const Point, const int32_t len, const uint32_t color);

void Vram_Clear(const Vram, const uint32_t color);

void Vram_DrawTile(const Vram, const Tile);

void Vram_DrawMap(const Vram, const Registrar, const Map, const Overview, const Grid, const Blendomatic, const Lines, const Tiles);

void Vram_DrawUnits(const Vram, const Tiles);

void Vram_DrawMouseTileSelect(const Vram, const Registrar, const Overview, const Grid);

void Vram_DrawUnitSelections(const Vram, const Tiles);

void Vram_DrawSelectionBox(const Vram, const Overview, const uint32_t color, const bool enabled);

void Vram_DrawUnitHealthBars(const Vram, const Tiles);

void Vram_DrawMotiveRow(const Vram, const Registrar, const Motive, const Color, const int32_t age);

void Vram_DrawHud(const Vram, const Registrar);

void Vram_Free(const Vram);
