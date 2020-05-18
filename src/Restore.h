#pragma once

#include "Unit.h"
#include "Share.h"
#include "Color.h"

#include <SDL2/SDL_net.h>
#include <stdbool.h>

#define RESTORE_CLIENT_ACK (0xAA)
#define RESTORE_SERVER_ACK (0xAB)

typedef struct
{
    Unit* unit;
    int32_t count;
    int32_t cycles;
    int32_t id_next;
    int32_t command_group_next;
    Share share[COLOR_COUNT];
    bool is_success;
}
Restore;

Restore Restore_Recv(const TCPsocket);

void Restore_Send(const Restore, const TCPsocket);

void Restore_Free(const Restore);

void Restore_Print(const Restore);
