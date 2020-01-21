#pragma once

#include "Input.h"
#include "Point.h"
#include "Grid.h"
#include "Event.h"
#include "Color.h"
#include "Rect.h"
#include "Motive.h"
#include "Quad.h"
#include "Status.h"
#include "Share.h"

#include <SDL2/SDL_net.h>

typedef struct
{
    Point pan;
    Rect selection_box;
    int32_t xres;
    int32_t yres;
    Point mouse_cursor;
    Event event;
    uint64_t parity;
    int32_t cycles;
    int32_t queue_size;
    int32_t ping;
    int32_t map_power;
    int32_t seed;
    int32_t users;
    Share share;
}
Overview;

Overview Overview_Init(const int32_t xres, const int32_t yres);

Overview Overview_Update(Overview, const Input, const uint64_t parity, const int32_t cycles, const int32_t queue_size, const Share, const int32_t ping);

Point Overview_IsoToCart(const Overview, const Grid, const Point, const bool raw);

Point Overview_CartToIso(const Overview, const Grid, const Point);

Quad Overview_GetRenderBox(const Overview, const Grid, const int32_t border);

Point Overview_IsoSnapTo(const Overview, const Grid, const Point);

bool Overview_IsSelectionBoxBigEnough(const Overview);

bool Overview_UsedAction(const Overview);
