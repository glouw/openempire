#include "Color.h"

static const char* strings[] = {
    "Blue", "Red", "Green", "Yellow", "Orange", "Cyan", "Purple", "Grey", "Gaia",
};

static const uint32_t colors[] = {
    0xFF0000FF,
    0xFFFF0000,
    0xFF00FF00,
    0xFFFFFF00,
    0xFFFFA500,
    0xFF00FFFF,
    0xFF800080,
    0xFFD3D3D3,
    0xFF000000,
};

const char* Color_ToString(const Color color)
{
    return strings[color];
}

uint32_t Color_ToInt(const Color color)
{
    return colors[color];
}
