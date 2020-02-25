#include "Ping.h"

#include "Sock.h"
#include "Args.h"
#include "Util.h"

#include <SDL2/SDL_mutex.h>

static SDL_mutex* mutex;

static int32_t ping;

static bool is_running = false;

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

static int32_t Ping(void* const data)
{
    Args* args = (Args*) data;
    const Sock pinger = Sock_Connect(args->host, args->port_ping);
    while(true)
    {
        const uint8_t send = 0xAF;
        uint8_t temp = 0x0;
        const int32_t t0 = SDL_GetTicks();
        UTIL_TCP_SEND(pinger.server, &send);
        UTIL_TCP_RECV(pinger.server, &temp);
        const int32_t t1 = SDL_GetTicks();
        if(temp == send)
            Set(t1 - t0);
    }
    Sock_Disconnect(pinger);
    return 0;
}

void Ping_Init(const Args args)
{
    if(!is_running)
    {
        mutex = SDL_CreateMutex();
        SDL_CreateThread(Ping, "N/A", (void*) &args);
        is_running = true;
    }
}

void Ping_Shutdown(void)
{
    SDL_DestroyMutex(mutex);
}
