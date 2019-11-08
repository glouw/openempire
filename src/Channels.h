#pragma once

#include "Tiles.h"
#include "Overview.h"

#include <stdint.h>

typedef struct
{
    Tiles* tiles;
    int32_t count;
}
Channels;

Channels Channels_Make(const Tiles, const Overview);

void Channels_Free(const Channels);
