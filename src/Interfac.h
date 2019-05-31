#pragma once

#include "File.h"

typedef enum
{
#define FILE_X(name, file, prio, walkable, type, interfac) name = file,
    FILE_X_INTERFAC
#undef FILE_X
}
Interfac;

const char* Interfac_GetString(const Interfac);

uint8_t Interfac_GetHeight(const Interfac);
