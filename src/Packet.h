#pragma once

#include "Overview.h"
#include "Sock.h"
#include "Color.h"

#include <stdbool.h>
#include <SDL2/SDL_net.h>

// NO POINTERS ANYWHERE IN HERE! PERFECT FOR TCP...

typedef struct
{
    Overview overview[COLOR_COUNT];
    int32_t turn;
    int32_t exec_cycle;
    int16_t client_id;
    int16_t users_connected;
    int16_t users;
    uint16_t seed;
    int16_t control;
    bool is_stable;
    bool is_out_of_sync;
    bool game_running;
}
Packet;

Packet Packet_Get(const Sock);

Packet Packet_ZeroOverviews(Packet);

bool Packet_IsReady(const Packet);

void Packet_Flush(const Sock);

bool Packet_IsAlive(const Packet);
