#pragma once

#include "Type.h"
#include "Graphics.h"
#include "Point.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    const char* file_name;
    Point dimensions;
    Type type;
    int32_t max_speed;
    int32_t accel;
    int32_t max_health;
    int32_t attack;
    int32_t width;
    bool is_rotatable;
    bool is_single_frame;
    bool is_walkable;
    bool is_multi_state;
    bool can_expire;
}
Trait;

Trait Trait_Build(const Graphics);
