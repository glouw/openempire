#pragma once

#include <stdint.h>

typedef enum
{
#if 1
    COLOR_BLU,
    COLOR_RED,
#else
    COLOR_BLU,
    COLOR_RED,
    COLOR_GRN,
    COLOR_YEL,
    COLOR_ORG,
    COLOR_CYN,
    COLOR_PRP,
    COLOR_GRY,
#endif
    COLOR_COUNT,
}
Color;
