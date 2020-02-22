#include "Sockets.h"

#include "Config.h"
#include "Util.h"
#include "Restore.h"

#include <stdlib.h>
#include <stdbool.h>

Sockets Sockets_Init(const int32_t port)
{
    static Sockets zero;
    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, port);
    Sockets sockets = zero;
    sockets.self = SDLNet_TCP_Open(&ip);
    if(sockets.self == NULL)
        Util_Bomb("COULD NOT OPEN SERVER SOCKET\n");
    sockets.set = SDLNet_AllocSocketSet(COLOR_COUNT);
    return sockets;
}

void Sockets_Free(const Sockets sockets)
{
    SDLNet_TCP_Close(sockets.self);
    SDLNet_FreeSocketSet(sockets.set);
}

static Sockets Add(Sockets sockets, TCPsocket socket)
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
            TCPsocket socket = sockets.socket[i];
            if(SDLNet_SocketReady(socket))
            {
                static Overview zero;
                Overview overview = zero;
                const int32_t max = sizeof(overview);
                const int32_t bytes = SDLNet_TCP_Recv(socket, &overview, max);
                if(bytes <= 0)
                {
                    SDLNet_TCP_DelSocket(sockets.set, socket);
                    sockets.socket[i] = NULL;
                }
                if(bytes == max)
                {
                    cache->cycles[i] = overview.cycles;
                    cache->parity[i] = overview.parity;
                    cache->queue_size[i] = overview.queue_size;
                    cache->pings[i] = overview.ping;
                    if(Overview_UsedAction(overview))
                        cache->packet.overview[i] = overview;
                    Cache_CheckParity(cache);
                }
            }
        }
    return sockets;
}

static void Print(const Sockets sockets, Cache* const cache, const int32_t setpoint, const int32_t max_ping)
{
    printf("TURN %d :: SETPOINT %d :: MAX_PING %d\n", cache->turn, setpoint, max_ping);
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const uint64_t parity = cache->parity[i];
        const int32_t cycles = cache->cycles[i];
        const int32_t ping = cache->pings[i];
        const char control = cache->control[i];
        const char queue_size = cache->queue_size[i];
        const char parity_symbol = cache->is_stable ? '!' : '?';
        TCPsocket socket = sockets.socket[i];
        printf("%d :: %d :: %c :: 0x%016lX :: %c :: CYCLES %d :: QUEUE %d -> %d ms\n",
                i, socket != NULL, parity_symbol, parity, control, cycles, queue_size, ping);
    }
}

static void Send(const Sockets sockets, Cache* const cache, const int32_t max_cycle, const int32_t max_ping, const bool game_running)
{
    const int32_t dt_cycles = max_ping / CONFIG_MAIN_LOOP_SPEED_MS;
    const int32_t buffer = 3;
    const int32_t exec_cycle = max_cycle + dt_cycles + buffer;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        TCPsocket socket = sockets.socket[i];
        if(socket)
        {
            Packet packet = cache->packet;
            packet.control = cache->control[i];
            packet.turn = cache->turn;
            packet.exec_cycle = exec_cycle;
            packet.client_id = i;
            packet.is_stable = cache->is_stable;
            packet.is_out_of_sync = cache->is_out_of_sync;
            packet.game_running = game_running;
            packet.users_connected = cache->users_connected;
            packet.users = cache->users;
            packet.seed = cache->seed;
            packet.map_power = cache->map_power;
            if(!cache->is_stable)
                packet = Packet_ZeroOverviews(packet);
            SDLNet_TCP_Send(socket, &packet, sizeof(packet));
        }
    }
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
        TCPsocket socket = sockets.socket[i];
        if(socket != NULL)
            count++;
    }
    return count;
}

void Sockets_Send(const Sockets sockets, Cache* const cache, const int32_t cycles, const bool quiet)
{
    if(ShouldSend(cycles, CONFIG_SOCKETS_SERVER_UPDATE_SPEED_CYCLES))
    {
        const int32_t setpoint = Cache_GetCycleSetpoint(cache);
        const int32_t max_cycle = Cache_GetCycleMax(cache);
        const int32_t max_ping = Cache_GetPingMax(cache);
        Cache_CalculateControlChars(cache, setpoint);
        Cache_CheckStability(cache, setpoint);
        cache->users_connected = CountConnectedPlayers(sockets);
        const bool game_running = Cache_GetGameRunning(cache);
        if(!quiet)
            Print(sockets, cache, setpoint, max_ping);
        Send(sockets, cache, max_cycle, max_ping, game_running);
        Cache_Clear(cache);
    }
}

Sockets Sockets_Accept(const Sockets sockets)
{
    const TCPsocket client = SDLNet_TCP_Accept(sockets.self);
    return (client != NULL)
        ? Add(sockets, client)
        : sockets;
}

void Sockets_Ping(const Sockets pingers)
{
    if(SDLNet_CheckSockets(pingers.set, 0))
        for(int32_t i = 0; i < COLOR_COUNT; i++)
        {
            TCPsocket socket = pingers.socket[i];
            if(SDLNet_SocketReady(socket))
            {
                int32_t temp = 0;
                const int32_t size = sizeof(temp);
                SDLNet_TCP_Recv(socket, &temp, size);
                SDLNet_TCP_Send(socket, &temp, size);
            }
        }
}

void Sockets_Reset(const Sockets resets, Cache* const cache)
{
    if(SDLNet_CheckSockets(resets.set, 0))
    {
        TCPsocket socket = resets.socket[COLOR_BLU];
        if(SDLNet_SocketReady(socket))
        {
            const Restore restore = Restore_Recv(socket);
            for(int32_t i = 0; i < COLOR_COUNT; i++)
                Restore_Send(restore, resets.socket[i]);
            cache->is_out_of_sync = false;
            Restore_Free(restore);
        }
    }
}
