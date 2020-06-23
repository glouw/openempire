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
    uint64_t parity;
    Point pan;
    Rect selection_box;
    Point mouse_cursor;
    Event event;
    Color color;
    Color spectator;
    Share share;
    int32_t xres;
    int32_t yres;
    int32_t cycles;
    int16_t queue_size;
    int16_t ping;
    uint16_t seed;
    int16_t users;
}
Overview;

Overview Overview_Make(const int32_t xres, const int32_t yres);

Overview Overview_Update(Overview, const Input, const uint64_t parity, const int32_t cycles, const int32_t queue_size, const Share, const int32_t ping, const bool must_randomize_mouse);

Point Overview_IsoToCart(const Overview, const Grid, const Point, const bool raw);

Point Overview_CartToIso(const Overview, const Grid, const Point);

Quad Overview_GetRenderBox(const Overview, const Grid, const int32_t border);

Point Overview_IsoSnapTo(const Overview, const Grid, const Point);

bool Overview_IsSelectionBoxBigEnough(const Overview);

bool Overview_UsedAction(const Overview);

bool Overview_IsSpectator(const Overview);
