#include "Color.h"

static const char* strings[] = {
    "Blue", "Red", "Green", "Yellow", "Orange", "Cyan", "Purple", "Grey", "Gaia",
};

static const uint32_t colors[] = {
    0x0000FF,
    0xFF0000,
    0x00FF00,
    0xFFFF00,
    0xFFA500,
    0x00FFFF,
    0x800080,
    0xD3D3D3,
    0x000000,
};

const char* Color_ToString(const Color color)
{
    return strings[color];
}

uint32_t Color_ToInt(const Color color)
{
    return colors[color];
}
