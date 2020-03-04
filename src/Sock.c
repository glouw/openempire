#include "Sock.h"

#include "Util.h"

#include <SDL2/SDL_mutex.h>

static SDL_mutex* mutex;

void Sock_Setup(void)
{
    mutex = SDL_CreateMutex();
}

Sock Sock_Connect(const char* const host, const int32_t port)
{
    static Sock zero;
    Sock sock = zero;
    IPaddress ip;
    if(UTIL_LOCK(mutex))
    {
        const int32_t errors = SDLNet_ResolveHost(&ip, host, port);
        if(errors == -1)
            Util_Bomb("CLIENT :: COULD NOT RESOLVE HOST\n");
        UTIL_UNLOCK(mutex);
    }
    sock.server = SDLNet_TCP_Open(&ip);
    if(sock.server == NULL)
        Util_Bomb("CLIENT :: Could not connect to %s:%d... Is the openempires server running?\n", host, port);
    sock.set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(sock.set, sock.server);
    return sock;
}

void Sock_Disconnect(const Sock sock)
{
    SDLNet_FreeSocketSet(sock.set);
    SDLNet_TCP_Close(sock.server);
}
