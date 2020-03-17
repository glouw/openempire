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
    int32_t id_next;
    Share share[COLOR_COUNT];
}
Restore;

Restore Restore_Recv(const TCPsocket);

void Restore_Send(const Restore, const TCPsocket);

void Restore_Free(const Restore);
