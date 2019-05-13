#pragma once

#include "File.h"

typedef enum
{
#define FILE_X(name, file, prio) name = file,
    FILE_X_GRAPHICS
#undef FILE_X
}
Graphics;

const char* Graphics_GetString(const Graphics);

uint8_t Graphics_GetHeight(const Graphics);
