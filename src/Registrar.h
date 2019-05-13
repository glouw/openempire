#pragma once

#include "Animation.h"
#include "Drs.h"
#include "Color.h"

#include <SDL2/SDL.h>

typedef struct
{
    Animation* animation[COLOR_COUNT];
    const int32_t* files;
    int32_t file_count;
}
Registrar;

void Registrar_Free(const Registrar);

Registrar Registrar_LoadInterfac(const char* const path);

Registrar Registrar_LoadTerrain(const char* const path);

Registrar Registrar_LoadGraphics(const char* const path);
