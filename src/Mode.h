#pragma once

#include "Image.h"
#include "Frame.h"
#include "Point.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdint.h>

// Modes define blendomatic blend styles.

typedef struct
{
    uint32_t tile_size;
    uint8_t* tile_flags;
    uint8_t* tile_bit_mask;
    uint8_t* tile_byte_mask;
    SDL_Surface** mask_demo;
    SDL_Surface** mask_real;
    Frame frame;
}
Mode;

Mode Mode_Load(FILE* const, const int32_t nr_tiles, const Frame, Outline* const);

void Mode_Free(const Mode, const int32_t nr_tiles);

int32_t Mode_GetBlendIndex(const Point a, const Point b);
