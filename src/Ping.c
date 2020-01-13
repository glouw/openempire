#include "Ping.h"

#include "Sock.h"
#include "Args.h"

#include <SDL2/SDL_mutex.h>

static SDL_mutex* mutex;

static int32_t ping;

void Ping_Init(void)
{
    mutex = SDL_CreateMutex();
}

void Ping_Shutdown(void)
{
    SDL_DestroyMutex(mutex);
}

static void Set(const int32_t dt)
{
    if(SDL_TryLockMutex(mutex) == 0)
    {
        ping = dt;
        SDL_UnlockMutex(mutex);
    }
}

int32_t Ping_Get(void)
{
    int32_t out = -1;
    if(SDL_TryLockMutex(mutex) == 0)
    {
        out = ping;
        SDL_UnlockMutex(mutex);
    }
    return out;
}

int32_t Ping_Ping(void* const data)
{
    Args* args = (Args*) data;
    const Sock pinger = Sock_Connect(args->host, args->port_ping);
    while(true)
    {
        const int32_t message = 0xCAFEBABE;
        int32_t temp;
        const int32_t size = sizeof(temp);
        const int32_t t0 = SDL_GetTicks();
        SDLNet_TCP_Send(pinger.server, &message, size);
        SDLNet_TCP_Recv(pinger.server, &temp, size);
        const int32_t t1 = SDL_GetTicks();
        if(temp == message)
            Set(t1 - t0);
    }
    Sock_Disconnect(pinger);
    return 0;
}
