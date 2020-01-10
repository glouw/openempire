#include "Color.h"

static const char* strings[] = {
    "Blue",
    "Red",
    "Green",
    "Yellow",
    "Orange",
    "Cyan",
    "Purple",
    "Grey",
    "Gaia",
};

static const uint32_t colors[] = {
    0x000000FF,
    0x00FF0000,
    0x0000FF00,
    0x00FFFF00,
    0x00FFA500,
    0x0000FFFF,
    0x00800080,
    0x00D3D3D3,
    0x00000000,
};

const char* Color_ToString(const Color color)
{
    return strings[color];
}

uint32_t Color_ToInt(const Color color)
{
    return colors[color];
}
