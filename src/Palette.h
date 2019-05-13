#pragma once

#include "Drs.h"

#include <stdint.h>

typedef struct
{
    char label[16];
    char version[8];
    uint32_t* color;
    int32_t count;
}
Palette;

void Palette_Print(const Palette);

Palette Palette_Load(const Drs interfac, const int32_t palette_offset);

void Palette_Free(const Palette);
