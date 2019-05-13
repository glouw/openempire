#pragma once

#include "Registrar.h"
#include "Blendomatic.h"

#include <SDL2/SDL.h>

typedef struct
{
    Blendomatic blendomatic;
    Registrar terrain;
    Registrar graphics;
    Registrar interfac;
}
Data;

Data Data_Load(const char* const path);

void Data_Free(const Data);
