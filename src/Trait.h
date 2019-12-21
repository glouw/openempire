#pragma once

#include "Type.h"
#include "Graphics.h"
#include "Point.h"
#include "Action.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    const char* file_name;
    Point dimensions;
    Type type;
    int32_t max_speed;
    int32_t max_health;
    int32_t attack;
    int32_t width;
    Action action;
    bool is_single_frame;
    bool is_walkable;
    bool is_multi_state;
    bool is_inanimate;
    bool can_expire;
    bool is_detail;
    bool needs_midding;
}
Trait;

Trait Trait_Build(const Graphics);
