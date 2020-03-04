#include "Ping.h"

#include "Sock.h"
#include "Args.h"
#include "Util.h"

#include <SDL2/SDL_mutex.h>
#include <assert.h>

static SDL_mutex* mutex;

static int32_t ping;

static void Set(const int32_t dt)
{
    if(UTIL_LOCK(mutex))
    {
        ping = dt;
        UTIL_UNLOCK(mutex);
    }
}

int32_t Ping_Get(void)
{
    int32_t out = -1;
    if(UTIL_LOCK(mutex))
    {
        out = ping;
        UTIL_UNLOCK(mutex);
    }
    return out;
}

static int32_t Ping(void* const data)
{
    Args* const args = (Args*) data;
    const Sock pinger = Sock_Connect(args->host, args->port_ping);
    while(true)
    {
        const uint8_t send = 0xAF;
        uint8_t temp = 0x0;
        const int32_t t0 = SDL_GetTicks();
        assert(UTIL_TCP_SEND(pinger.server, &send) == 1);
        assert(UTIL_TCP_RECV(pinger.server, &temp) == 1);
        const int32_t t1 = SDL_GetTicks();
        const int32_t dt = t1 - t0;
        if(temp == send)
            Set(dt);
        const int32_t delay = 200 - dt;
        SDL_Delay(delay > 0 ? delay : 0);
    }
    Sock_Disconnect(pinger);
    return 0;
}

void Ping_Init(const Args args)
{
    mutex = SDL_CreateMutex();
    SDL_CreateThread(Ping, "N/A", (void*) &args); // NO POINTER IS RETURNED AS PING THREAD WILL SHUTDOWN WITH PARENT.
}

void Ping_Shutdown(void)
{
    SDL_DestroyMutex(mutex);
}
