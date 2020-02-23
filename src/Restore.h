#pragma once

#include "Unit.h"
#include "Share.h"
#include "Color.h"

#include <SDL2/SDL_net.h>

typedef struct
{
    Unit* unit;
    int32_t count;
    int32_t cycles;
    Share stamp[COLOR_COUNT];
}
Restore;

Restore Restore_Recv(TCPsocket);

void Restore_Send(const Restore, TCPsocket);

void Restore_Free(const Restore);
