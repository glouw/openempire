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

Sockets Sockets_Service(const Sockets, Cache* const);

void Sockets_Relay(const Sockets, const int32_t cycles, Cache* const);

Sockets Sockets_Accept(const Sockets);

void Sockets_Ping(const Sockets);

void Sockets_Panic(const Sockets, Cache* const);

int32_t Sockets_CountConnectedPlayers(const Sockets);

bool Sockets_IsOutOfSync(const Sockets);
