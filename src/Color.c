#include "Color.h"

static const char* const strings[] = {
    [COLOR_BLU ] = "Blue",
    [COLOR_RED ] = "Red",
    [COLOR_GRN ] = "Green",
    [COLOR_YEL ] = "Yellow",
    [COLOR_ORG ] = "Orange",
    [COLOR_CYN ] = "Cyan",
    [COLOR_PRP ] = "Purple",
    [COLOR_GRY ] = "Grey",
    [COLOR_GAIA] = "Gaia",
};

static const uint32_t colors[] = {
    [COLOR_BLU ] = 0x000000FF,
    [COLOR_RED ] = 0x00FF0000,
    [COLOR_GRN ] = 0x0000FF00,
    [COLOR_YEL ] = 0x00FFFF00,
    [COLOR_ORG ] = 0x00FFA500,
    [COLOR_CYN ] = 0x0000FFFF,
    [COLOR_PRP ] = 0x00800080,
    [COLOR_GRY ] = 0x00D3D3D3,
    [COLOR_GAIA] = 0x00000000,
};

const char* Color_ToString(const Color color)
{
    return strings[color];
}

uint32_t Color_ToInt(const Color color)
{
    return colors[color];
}
