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
    COLOR_COUNT,
}
Color;

void Color_SetMyColor(const Color);

Color Color_GetMyColor(void);
