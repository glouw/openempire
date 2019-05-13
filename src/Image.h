#pragma once

#include "Outline.h"
#include "Frame.h"
#include "Drs.h"
#include "Palette.h"
#include "Color.h"

#include <stdint.h>
#include <stdio.h>

typedef struct
{
    Outline* outline_table;
    uint32_t* cmd_table;
    uint8_t* data;
    int32_t height;
    int32_t size;
}
Image;

Image Image_Load(const int32_t height, const int32_t size, FILE* const fp);

void Image_Print(const Image);

void Image_Free(const Image);

SDL_Surface* Image_Parse(const Image, const Frame, const Palette, const Color);

Image Image_Copy(const Image);
