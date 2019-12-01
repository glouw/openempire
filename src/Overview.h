#pragma once

#include "Input.h"
#include "Point.h"
#include "Grid.h"
#include "Color.h"
#include "Rect.h"
#include "Quad.h"

#include <SDL2/SDL_net.h>

typedef struct
{
    bool mouse_l        : 1;
    bool mouse_r        : 1;
    bool mouse_ld       : 1;
    bool mouse_lu       : 1;
    bool mouse_rd       : 1;
    bool mouse_ru       : 1;
    bool key_left_shift : 1;
    bool key_q          : 1;
    bool key_w          : 1;
    bool key_e          : 1;
    bool key_r          : 1;
    bool key_t          : 1;
    bool key_a          : 1;
    bool key_s          : 1;
    bool key_d          : 1;
    bool key_f          : 1;
    bool key_g          : 1;
    bool key_z          : 1;
    bool key_x          : 1;
    bool key_c          : 1;
    bool key_v          : 1;
    bool key_b          : 1;
    bool key_1          : 1;
    bool key_2          : 1;
    bool key_3          : 1;
    bool key_left_ctrl  : 1;
}
Event;

typedef struct
{
    Point pan;
    Rect selection_box;
    int32_t xres;
    int32_t yres;
    Color color;
    Point mouse_cursor;
    Event event;
    uint64_t parity;
    int32_t cycles;
}
Overview;

Overview Overview_Init(const Color, const int32_t xres, const int32_t yres);

Overview Overview_Update(Overview, const Input, const uint64_t parity, const int32_t cycles);

Point Overview_IsoToCart(const Overview, const Grid, const Point, const bool raw);

Point Overview_CartToIso(const Overview, const Grid, const Point);

Quad Overview_GetRenderBox(const Overview, const Grid, const int32_t border);

Point Overview_IsoSnapTo(const Overview, const Grid, const Point);

bool Overview_IsSelectionBoxBigEnough(const Overview);

bool Overview_UsedAction(const Overview);
