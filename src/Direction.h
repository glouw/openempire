#pragma once

#include "Point.h"

#include <stdbool.h>
#include <stdlib.h>

typedef enum
{
    DIRECTION_S,
    DIRECTION_SW,
    DIRECTION_W,
    DIRECTION_NW,
    DIRECTION_N,
    DIRECTION_NE,
    DIRECTION_E,
    DIRECTION_SE,
    DIRECTION_COUNT
}
Direction;

Direction Direction_Fix(const Direction, bool* const flip_vert);

Direction Direction_GetCart(const Point);

Direction Direction_CartToIso(const Direction);
