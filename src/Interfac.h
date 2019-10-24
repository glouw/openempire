#pragma once

#include "File.h"

typedef enum
{
#define FILE_X(name, file, prio, walkable, type, max_speed, health, attack, width, single_frame, multi_state, expire, inanimate, dimensions, action, detail) name = file,
    FILE_X_INTERFAC
#undef FILE_X
}
Interfac;

const char* Interfac_GetString(const Interfac);

uint8_t Interfac_GetHeight(const Interfac);
