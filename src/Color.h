#pragma once

#include <stdint.h>

#if 1 /* Set to 1 for loading test colors, else 0 */
#define COLOR_X_MACRO \
    COLOR_X(COLOR_BLU, 0x000000FF) \
    COLOR_X(COLOR_RED, 0x00FF0000)
#else
#define COLOR_X_MACRO \
    COLOR_X(COLOR_BLU, 0x000000FF) \
    COLOR_X(COLOR_RED, 0x00FF0000) \
    COLOR_X(COLOR_GRN, 0x0000FF00) \
    COLOR_X(COLOR_YEL, 0x00000000) \
    COLOR_X(COLOR_ORG, 0x00000000) \
    COLOR_X(COLOR_CYN, 0x00000000) \
    COLOR_X(COLOR_PRP, 0x00000000) \
    COLOR_X(COLOR_GRY, 0x00000000)
#endif

typedef enum
{
#define COLOR_X(name, hex) name,
    COLOR_X_MACRO
#undef COLOR_X
    COLOR_COUNT
}
Color;

uint32_t Color_ToRgb(const Color);
