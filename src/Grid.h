#pragma once

#include <stdint.h>

typedef struct
{
    int32_t cols;
    int32_t rows;
    int32_t tile_iso_width;
    int32_t tile_iso_height;
    int32_t tile_cart_width;
    int32_t tile_cart_height;
}
Grid;

Grid Grid_Make(const int32_t cols, const int32_t rows, const int32_t tile_iso_width, const int32_t tile_iso_height);
