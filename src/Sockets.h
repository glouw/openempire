#pragma once

#include "Cache.h"
#include "Config.h"
#include "Backup.h"

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL_net.h>

typedef struct
{
    TCPsocket socket[COLOR_COUNT];
    TCPsocket self;
    SDLNet_SocketSet set;
}
Sockets;

Sockets Sockets_Init(const int32_t port);

void Sockets_Free(const Sockets);

void Sockets_Send(const Sockets, const int32_t cycles, Cache* const);

Sockets Sockets_Accept(const Sockets);

void Sockets_Ping(const Sockets);

void Sockets_Panic(const Sockets, Cache* const);

Sockets Sockets_Recieve(Sockets, Cache* const);

void Sockets_CountConnectedPlayers(Sockets, Cache* const);
