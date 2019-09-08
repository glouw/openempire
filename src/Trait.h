#pragma once

#include "Type.h"
#include "Graphics.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    int32_t max_speed;
    int32_t accel;
    const char* file_name;
    int32_t max_health;
    int32_t attack;
    bool is_rotatable;
    bool is_single_frame;
    bool is_walkable;
    int32_t width;
    Type type;
    bool is_multi_state;
    bool can_expire;
}
Trait;

Trait Trait_Build(const Graphics);
