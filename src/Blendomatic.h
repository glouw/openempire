#pragma once

#include "Mode.h"
#include "Registrar.h"

#include <stdint.h>

typedef struct
{
    uint32_t nr_blending_modes;
    uint32_t nr_tiles;
    Mode* mode;
}
Blendomatic;

Blendomatic Blendomatic_Load(const char* const path, const Registrar terrain);

void Blendomatic_Free(const Blendomatic);
