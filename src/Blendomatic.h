#pragma once

#include "Mode.h"
#include "Registrar.h"

#include <stdint.h>

typedef struct
{
    Mode* mode;
    uint32_t nr_blending_modes;
    uint32_t nr_tiles;
}
Blendomatic;

Blendomatic Blendomatic_Load(const char* const path, const Registrar terrain);

void Blendomatic_Free(const Blendomatic);
