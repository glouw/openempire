#pragma once

#include "Unit.h"

#include <SDL2/SDL_net.h>

typedef struct
{
    Unit* unit;
    int32_t count;
    int32_t cycles;
}
Restore;

Restore Restore_Recv(TCPsocket);

void Restore_Send(const Restore, TCPsocket);

void Restore_Free(const Restore);
