#pragma once

#include "Packet.h"
#include "Parity.h"

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL_net.h>

typedef struct
{
    int32_t queue_size[COLOR_COUNT];
    int32_t pings[COLOR_COUNT];
    Parity parity[COLOR_COUNT];
    TCPsocket socket[COLOR_COUNT];
    char control[COLOR_COUNT];
    TCPsocket self;
    Packet packet;
    SDLNet_SocketSet set;
    int32_t turn;
    int32_t seed;
    bool is_stable;
}
Sockets;

Sockets Sockets_Init(const int32_t port);

void Sockets_Free(Sockets);

Sockets Sockets_Service(const Sockets);

Sockets Sockets_Relay(const Sockets, const int32_t cycles, const bool quiet, const int32_t users, const int32_t map_power);

Sockets Sockets_Accept(const Sockets);

void Sockets_Ping(const Sockets);
