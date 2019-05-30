#pragma once

#include "Point.h"

#include <stdint.h>

typedef struct
{
    int32_t cols;
    int32_t rows;
    int32_t tile_iso_width;
    int32_t tile_iso_height;
    int32_t tile_cart_width;
    int32_t tile_cart_height;
    int32_t cell_size;
}
Grid;

Grid Grid_Make(const int32_t cols, const int32_t rows, const int32_t tile_iso_width, const int32_t tile_iso_height);

Point Grid_GetCoords(const Grid, const Point);

Point Grid_GetOffset(const Grid, const Point);

Point Grid_CellToOffset(const Grid, const Point);

Point Grid_CellToCart(const Grid, const Point);

Point Grid_CartToCell(const Grid, const Point);
