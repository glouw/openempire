#pragma once

#include <stdint.h>

typedef struct
{
    int32_t cols;
    int32_t rows;
    int32_t tile_width;
    int32_t tile_height;
    int32_t cart_width;
    int32_t cart_height;
}
Grid;

Grid Grid_Make(const int32_t cols, const int32_t rows, const int32_t tile_width, const int32_t tile_height);
