#pragma once

#include "Packet.h"

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL_net.h>

typedef struct
{
    TCPsocket socket[COLOR_COUNT];
    TCPsocket self;
    Packet packet;
    SDLNet_SocketSet set;
}
Sockets;

Sockets Sockets_Init(const int32_t port);

void Sockets_Free(Sockets);

Sockets Sockets_Service(const Sockets, const int32_t timeout);

Sockets Sockets_Relay(const Sockets, const int32_t cycles, const int32_t interval);

Sockets Sockets_Accept(const Sockets);
