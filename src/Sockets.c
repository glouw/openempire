#include "Sockets.h"

#include "Config.h"
#include "Util.h"
#include "Restore.h"

#include <SDL2/SDL_mutex.h>
#include <stdlib.h>
#include <stdbool.h>

static SDL_mutex* mutex;

void Sockets_Init(void)
{
    mutex = SDL_CreateMutex();
}

Sockets Sockets_Make(const int32_t port)
{
    static Sockets zero;
    Sockets sockets = zero;
    IPaddress ip;
    if(UTIL_LOCK(mutex))
    {
        SDLNet_ResolveHost(&ip, NULL, port);
        UTIL_UNLOCK(mutex);
    }
    sockets.self = SDLNet_TCP_Open(&ip);
    if(sockets.self == NULL)
        Util_Bomb("SERVER :: COULD NOT OPEN SERVER SOCKET\n");
    sockets.set = SDLNet_AllocSocketSet(COLOR_COUNT);
    return sockets;
}

void Sockets_Free(const Sockets sockets)
{
    SDLNet_TCP_Close(sockets.self);
    SDLNet_FreeSocketSet(sockets.set);
}

static Sockets Add(Sockets sockets, const TCPsocket socket)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        if(sockets.socket[i] == NULL)
        {
            SDLNet_TCP_AddSocket(sockets.set, socket);
            sockets.socket[i] = socket;
            return sockets;
        }
    return sockets;
}

Sockets Sockets_Recv(Sockets sockets, Cache* const cache)
{
    if(SDLNet_CheckSockets(sockets.set, 0))
        for(int32_t i = 0; i < COLOR_COUNT; i++)
        {
            const TCPsocket socket = sockets.socket[i];
            if(SDLNet_SocketReady(socket))
            {
                static Overview zero;
                Overview overview = zero;
                const int32_t bytes = UTIL_TCP_RECV_NO_ASSERT(socket, &overview);
                if(bytes <= 0)
                {
                    SDLNet_TCP_DelSocket(sockets.set, socket);
                    sockets.socket[i] = NULL;
                }
                if(bytes == sizeof(overview))
                {
                    const int32_t count = cache->history[i].count++;
                    cache->history[i].cycles[count] = cache->cycles[i] = overview.cycles;
                    cache->history[i].parity[count] = cache->parity[i] = overview.parity;
                    cache->queue_size[i] = overview.queue_size;
                    cache->pings[i] = overview.ping;
                    if(Overview_UsedAction(overview))
                        cache->packet.overview[i] = overview;
                    if(count == CONFIG_HISTORY_COUNT)
                        cache->history[i].count = 0;
                }
            }
        }
    Cache_CheckParity(cache);
    return sockets;
}

static void Print(const Sockets sockets, Cache* const cache, const int32_t setpoint, const int32_t max_ping, const bool game_running)
{
    printf("TURN %d :: SETPOINT %d :: MAX_PING %d :: RESTORE_COUNT %d\n", cache->turn, setpoint, max_ping, cache->restore_count);
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const uint64_t parity    = cache->parity[i];
        const int32_t cycles     = cache->cycles[i];
        const int32_t ping       = cache->pings[i];
        const int32_t control    = cache->control[i];
        const int32_t integral   = cache->integral[i];
        const int32_t queue_size = cache->queue_size[i];
        const char parity_symbol = cache->is_stable ? '!' : '?';
        const TCPsocket socket = sockets.socket[i];
        printf(/*  0 */ "%d :: "
               /*  1 */ "%d :: "
               /*  2 */ "%d :: "
               /*  3 */ "%c :: "
               /*  4 */ "0x%016lX :: "
               /*  5 */ "QUEU %d :: "
               /*  6 */ "PING %3d :: "
               /*  7 */ "OVER %ld :: "
               /*  8 */ "PACK %ld ::"
               /*  9 */ "CTRL %d :: "
               /* 10 */ "INTG %3d :: "
               /* 11 */ "CYCS %d\n",
               /*  0 */ game_running,
               /*  1 */ i,
               /*  2 */ socket != NULL,
               /*  3 */ parity_symbol,
               /*  4 */ parity,
               /*  5 */ queue_size,
               /*  6 */ ping,
               /*  7 */ sizeof(Overview),
               /*  8 */ sizeof(Packet),
               /*  9 */ control,
               /* 10 */ integral,
               /* 11 */ cycles);
    }
}

typedef struct
{
    TCPsocket socket;
    Packet packet;
}
SendNeedle;

static int32_t RunSendNeedle(void* const data)
{
    SendNeedle* const needle = (SendNeedle*) data;
    if(needle->socket)
        UTIL_TCP_SEND(needle->socket, &needle->packet);
    return 0;
}

static void Send(const Sockets sockets, Cache* const cache, const int32_t max_cycle, const int32_t max_ping, const bool game_running)
{
    const int32_t dt_cycles = max_ping / CONFIG_MAIN_LOOP_SPEED_MS;
    const int32_t buffer = 3; // XXX. THIS BUFFER NEEDS TO BE LARGER IF ONE OF THE CLIENT IS DRIVING HARDER WITH ITS CONTROL VALUE.
    const int32_t exec_cycle = max_cycle + dt_cycles + buffer;
    Packet base = cache->packet;
    base.turn = cache->turn;
    base.exec_cycle = exec_cycle;
    base.is_stable = cache->is_stable;
    base.is_out_of_sync = cache->is_out_of_sync;
    base.game_running = game_running;
    base.users_connected = cache->users_connected;
    base.users = cache->users;
    base.seed = cache->seed;
    base.map_size = cache->map_size;
    if(!cache->is_stable || cache->is_out_of_sync)
        base = Packet_ZeroOverviews(base);
    SendNeedle  needles[COLOR_COUNT]; // THREADS ENSURE ALL CLIENTS GET THEIR PACKETS ROUGHLY AT THE SAME TIME.
    SDL_Thread* threads[COLOR_COUNT]; // FOR EXAMPLE, A SLIGHT HANG ON CLIENT 2 WILL NOT DELAY CLIENT 7 AND MISS ITS EXEC CYCLE DEADLINE.
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        Packet packet = base;
        const TCPsocket socket = sockets.socket[i];
        needles[i].socket = socket;
        if(socket)
        {
            packet.control = cache->control[i];
            packet.client_id = i;
            needles[i].packet = packet;
        }
    }
    for(int32_t i = 0; i < COLOR_COUNT; i++) threads[i] = SDL_CreateThread(RunSendNeedle, "N/A", &needles[i]);
    for(int32_t i = 0; i < COLOR_COUNT; i++) SDL_WaitThread(threads[i], NULL);
}

static bool ShouldSend(const int32_t cycles, const int32_t interval)
{
    return (cycles % interval) == 0;
}

static int32_t CountConnectedPlayers(const Sockets sockets)
{
    int32_t count = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const TCPsocket socket = sockets.socket[i];
        if(socket != NULL)
            count++;
    }
    return count;
}

void Sockets_Send(const Sockets sockets, Cache* const cache, const int32_t cycles, const bool quiet)
{
    if(ShouldSend(cycles, CONFIG_SOCKETS_SERVER_UPDATE_SPEED_CYCLES))
    {
        const int32_t max_cycle = Cache_GetCycleMax(cache);
        const int32_t min_cycle = Cache_GetCycleMin(cache);
        if(max_cycle - min_cycle > CONFIG_PLAYER_CYCLE_WINDOW)
            cache->is_out_of_sync = true;
        const int32_t setpoint = Cache_GetCycleSetpoint(cache);
        const int32_t max_ping = Cache_GetPingMax(cache);
        Cache_CalculateControl(cache, setpoint);
        Cache_CheckStability(cache, setpoint);
        cache->users_connected = CountConnectedPlayers(sockets);
        const bool game_running = Cache_GetGameRunning(cache);
        if(!quiet)
            Print(sockets, cache, setpoint, max_ping, game_running);
        Send(sockets, cache, max_cycle, max_ping, game_running);
        Cache_ClearPacket(cache);
    }
}

Sockets Sockets_Accept(Sockets sockets)
{
    TCPsocket client = NULL;
    if(UTIL_LOCK(mutex))
    {
        client = SDLNet_TCP_Accept(sockets.self);
        UTIL_UNLOCK(mutex);
    }
    if(client)
        sockets = Add(sockets, client);
    return sockets;
}

void Sockets_Ping(const Sockets pingers)
{
    if(SDLNet_CheckSockets(pingers.set, 0))
        for(int32_t i = 0; i < COLOR_COUNT; i++)
        {
            const TCPsocket socket = pingers.socket[i];
            if(SDLNet_SocketReady(socket))
            {
                uint8_t temp = 0;
                UTIL_TCP_RECV(socket, &temp);
                UTIL_TCP_SEND(socket, &temp);
            }
        }
}

typedef struct
{
    TCPsocket socket;
    Restore restore;
}
RestoreNeedle;

static int32_t RunRestoreNeedle(void* const data)
{
    RestoreNeedle* const needle = (RestoreNeedle*) data;
    if(needle->socket)
        Restore_Send(needle->restore, needle->socket);
    return 0;
}

void Sockets_Reset(const Sockets resets, Cache* const cache)
{
    if(SDLNet_CheckSockets(resets.set, 0))
        for(int32_t j = 0; j < COLOR_COUNT; j++)
        {
            const TCPsocket socket = resets.socket[j];
            if(SDLNet_SocketReady(socket))
            {
                const Restore restore = Restore_Recv(socket);
                if(restore.is_success)
                {
                    RestoreNeedle needles[COLOR_COUNT];
                    SDL_Thread*   threads[COLOR_COUNT];
                    for(int32_t i = 0; i < COLOR_COUNT; i++)
                    {
                        needles[i].socket = resets.socket[i];
                        needles[i].restore = restore;
                    }
                    for(int32_t i = 0; i < COLOR_COUNT; i++) threads[i] = SDL_CreateThread(RunRestoreNeedle, "N/A", &needles[i]);
                    for(int32_t i = 0; i < COLOR_COUNT; i++) SDL_WaitThread(threads[i], NULL);
                    Restore_Free(restore);
                    Cache_ClearHistory(cache);
                    printf("RESTORE TRANSMITTED TO CYCLE NUMBER %d\n", restore.cycles);
                    cache->restore_count += 1;
                    return;
                }
            }
        }
}
