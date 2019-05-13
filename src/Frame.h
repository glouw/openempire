#pragma once

#include "Point.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct
{
    uint32_t cmd_table_offset;
    uint32_t outline_table_offset;
    uint32_t palette_offset;
    uint32_t properties;
    int32_t width;
    int32_t height;
    int32_t hotspot_x;
    int32_t hotspot_y;
}
Frame;

Frame Frame_Load(FILE* const fp);

void Frame_Print(const Frame);
