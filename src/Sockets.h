#pragma once

#include "Cache.h"

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL_net.h>

typedef struct
{
    TCPsocket self;
    TCPsocket socket[COLOR_COUNT];
    SDLNet_SocketSet set;
}
Sockets;

Sockets Sockets_Init(const int32_t port);

void Sockets_Free(const Sockets);

Sockets Sockets_Recv(const Sockets, Cache* const);

void Sockets_Send(const Sockets, Cache* const, const int32_t cycles, const bool quiet);

Sockets Sockets_Accept(const Sockets);

void Sockets_Ping(const Sockets);

void Sockets_Reset(const Sockets, Cache* const);
