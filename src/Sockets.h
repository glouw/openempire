#pragma once

#include "Packet.h"

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL_net.h>

typedef struct
{
    int32_t cycles[COLOR_COUNT];
    int32_t queue_size[COLOR_COUNT];
    uint64_t parity[COLOR_COUNT];
    TCPsocket socket[COLOR_COUNT];
    char control[COLOR_COUNT];
    TCPsocket self;
    Packet packet;
    SDLNet_SocketSet set;
    int32_t turn;
    int32_t users_connected;
    int32_t users;
    bool is_stable;
}
Sockets;

Sockets Sockets_Init(const int32_t port, const int32_t users);

void Sockets_Free(Sockets);

Sockets Sockets_Service(const Sockets, const int32_t timeout);

Sockets Sockets_Relay(const Sockets, const int32_t cycles, const int32_t interval, const bool quiet);

Sockets Sockets_Accept(const Sockets);
