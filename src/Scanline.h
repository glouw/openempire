#pragma once

#include "Scanline.h"
#include "Outline.h"
#include "Palette.h"
#include "Image.h"
#include "Frame.h"
#include "Color.h"

#include <SDL2/SDL.h>

typedef struct
{
    int32_t* palette_index;
    int32_t count;
    int32_t max;
}
Scanline;

void Scanline_Free(const Scanline);

void Scanline_Draw(const Scanline, const int32_t y, const Outline, const Palette, SDL_Surface* const);

Scanline Scanline_Get(const Image, const Frame, const int32_t y, const Color);
