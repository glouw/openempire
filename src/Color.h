#pragma once

#include <stdint.h>

typedef enum
{
    COLOR_BLU,
    COLOR_RED,
    COLOR_GRN,
    COLOR_YEL,
    COLOR_ORG,
    COLOR_CYN,
    COLOR_PRP,
    COLOR_GRY,
    COLOR_GAIA,
    COLOR_COUNT,
}
Color;

const char* Color_ToString(const Color);

uint32_t Color_ToInt(const Color);
