#pragma once

#include "Tiles.h"
#include "Vram.h"

#include <stdint.h>

typedef struct
{
    Tiles* tiles;
    int32_t count;
}
Channels;

Channels Channels_Make(const Tiles, const Vram);

void Channels_Free(const Channels);
