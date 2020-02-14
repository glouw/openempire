#pragma once

#include "Overview.h"
#include "Sock.h"
#include "Color.h"

#include <stdbool.h>
#include <SDL2/SDL_net.h>

#define PACKET_CONTROL_SPEED_UP ('+')
#define PACKET_CONTROL_STEADY ('*')

// NO POINTERS ANYWHERE IN HERE! PERFECT FOR TCP...

typedef struct
{
    Overview overview[COLOR_COUNT];
    int32_t turn;
    int32_t exec_cycle;
    int32_t client_id;
    int32_t users_connected;
    int32_t users;
    int32_t seed;
    int32_t map_power;
    char control;
    bool is_stable;
    bool is_out_of_sync;
    bool game_running;
}
Packet;

Packet Packet_Get(const Sock);

Packet Packet_ZeroOverviews(Packet);

bool Packet_IsStable(const Packet);
