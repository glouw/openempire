#pragma once

#include "Overview.h"

#include <SDL2/SDL_net.h>

typedef struct
{
    TCPsocket server;
    SDLNet_SocketSet set;
}
Sock;

Sock Sock_Connect(const char* const host, const int32_t port, const char* const from);

void Sock_Disconnect(const Sock);

void Sock_Send(const Sock, const Overview);
