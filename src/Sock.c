#include "Sock.h"

#include "Util.h"

Sock Sock_Connect(const char* const host, const int32_t port)
{
    static Sock zero;
    Sock sock = zero;
    IPaddress ip;
    SDLNet_ResolveHost(&ip, host, port);
    sock.server = SDLNet_TCP_Open(&ip);
    if(sock.server == NULL)
        Util_Bomb("Could not connect to %s:%d... Is the openempires server running?\n", host, port);
    sock.set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(sock.set, sock.server);
    return sock;
}

void Sock_Disconnect(const Sock sock)
{
    SDLNet_FreeSocketSet(sock.set);
    SDLNet_TCP_Close(sock.server);
}

void Sock_Send(const Sock sock, const Overview overview)
{
    if(Overview_UsedAction(overview))
        SDLNet_TCP_Send(sock.server, &overview, sizeof(overview));
}
