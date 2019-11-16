#pragma once

#include "Input.h"
#include "Point.h"
#include "Grid.h"
#include "Color.h"
#include "Rect.h"
#include "Quad.h"

typedef struct
{
    Point pan;
    Rect selection_box;
    int32_t xres;
    int32_t yres;
    Color color;
    Point mouse_cursor;
    bool mouse_l;
    bool mouse_r;
    bool mouse_ld;
    bool mouse_lu;
    bool mouse_rd;
    bool mouse_ru;
    bool key_left_shift;
    bool key_q;
    bool key_w;
    bool key_e;
    bool key_r;
    bool key_t;
    bool key_a;
    bool key_s;
    bool key_d;
    bool key_f;
    bool key_g;
    bool key_z;
    bool key_x;
    bool key_c;
    bool key_v;
    bool key_b;
    bool key_1;
    bool key_2;
    bool key_3;
    bool key_left_ctrl;
    bool key_delete;
    bool key_return;
}
Overview;

Overview Overview_Init(const Color, const int32_t xres, const int32_t yres);

Overview Overview_Update(Overview, const Input);

Point Overview_IsoToCart(const Overview, const Grid, const Point, const bool raw);

Point Overview_CartToIso(const Overview, const Grid, const Point);

Quad Overview_GetRenderBox(const Overview, const Grid, const int32_t border);

Point Overview_IsoSnapTo(const Overview, const Grid, const Point);

bool Overview_IsSelectionBoxBigEnough(const Overview);
