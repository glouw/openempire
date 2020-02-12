#include "Sockets.h"

#include "Config.h"
#include "Util.h"
#include "Backup.h"
#include "Cache.h"

#include <stdlib.h>
#include <stdbool.h>

Sockets Sockets_Init(const int32_t port)
{
    static Sockets zero;
    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, port);
    Sockets sockets = zero;
    sockets.self = SDLNet_TCP_Open(&ip);
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

static Sockets Recieve(Sockets sockets, Cache* const cache)
{
    if(SDLNet_CheckSockets(sockets.set, CONFIG_SOCKETS_SERVER_TIMEOUT_MS))
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
                    Cache_AppendParity(cache, (Color) i, overview.parity);
                    cache->queue_size[i] = overview.queue_size;
                    cache->pings[i] = overview.ping;
                    if(Overview_UsedAction(overview))
                        cache->packet.overview[i] = overview;
                }
            }
        }
    return sockets;
}

static void CountConnectedPlayers(Sockets sockets, Cache* const cache)
{
    int32_t count = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        TCPsocket socket = sockets.socket[i];
        if(socket != NULL)
            count++;
    }
    cache->users_connected = count;
}

Sockets Sockets_Service(Sockets sockets, Cache* const cache)
{
    sockets = Recieve(sockets, cache);
    CountConnectedPlayers(sockets, cache);
    Cache_CalcOutOfSync(cache);
    return sockets;
}

static void Send(Sockets sockets, const int32_t max_cycle, const int32_t max_ping, Cache* const cache)
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
            packet.is_game_running = cache->users_connected == cache->users;
            packet.users_connected = cache->users_connected;
            packet.users = cache->users;
            packet.seed = cache->seed;
            packet.map_power = cache->map_power;
            packet.is_out_of_sync = cache->is_out_of_sync;
            if(!cache->is_stable)
                packet = Packet_ZeroOverviews(packet);
            SDLNet_TCP_Send(socket, &packet, sizeof(packet));
        }
    }
}

void Sockets_Relay(Sockets sockets, const int32_t cycles, Cache* const cache)
{
    if(cycles % CONFIG_SOCKETS_SERVER_UPDATE_SPEED_CYCLES == 0)
    {
        printf("<3 %d\n", cycles);
        const int32_t setpoint = Cache_GetCycleSetpoint(cache);
        const int32_t max_cycle = Cache_GetCycleMax(cache);
        const int32_t max_ping = Cache_GetPingMax(cache);
        Cache_CalcStability(cache, setpoint);
        Cache_CalculateControlChars(cache, setpoint);
        Send(sockets, max_cycle, max_ping, cache);
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

void Sockets_Ping(const Sockets ping)
{
    if(SDLNet_CheckSockets(ping.set, CONFIG_SOCKETS_SERVER_TIMEOUT_MS))
        for(int32_t i = 0; i < COLOR_COUNT; i++)
        {
            TCPsocket socket = ping.socket[i];
            if(SDLNet_SocketReady(socket))
            {
                int32_t temp = 0;
                const int32_t size = sizeof(temp);
                SDLNet_TCP_Recv(socket, &temp, size);
                SDLNet_TCP_Send(socket, &temp, size);
            }
        }
}

void Sockets_Panic(Sockets panic, Cache* const cache)
{
    if(cache->is_stable)
    {
        if(SDLNet_CheckSockets(panic.set, CONFIG_SOCKETS_SERVER_TIMEOUT_MS)) // PANICS ARE STREAMED IN BECAUSE CLIENTS ARE NOT PREDICTABLE.
            for(int32_t i = 0; i < COLOR_COUNT; i++)
            {
                TCPsocket socket = panic.socket[i];
                if(SDLNet_SocketReady(socket))
                {
                    printf("GOT %d :: PANIC COUNT %d\n", i, cache->panic_count);
                    SDLNet_TCP_Recv(socket, cache->panic[i], sizeof(cache->panic[i]));
                    cache->panic_count++;
                }
            }
        if(cache->panic_count == cache->users_connected) // IN A SINGLE BATCH CALL, ALL CLIENTS ARE UPDATED WITH THE INDEX THEY MUST FALL BACK WITH.
        {
            for(int32_t i = 0; i < COLOR_COUNT; i++)
            {
                int32_t index = -1;
                TCPsocket socket = panic.socket[i];
                SDLNet_TCP_Send(socket, &index, sizeof(index));
            }
            cache->panic_count = 0;
        }
        // for(int32_t i = 0; i < COLOR_COUNT; i++)
        // {
        //     for(int32_t j = 0; j < BACKUP_MAX; j++)
        //         printf("0x%016lX %5d ", parity[i][j].xorred, parity[i][j].cycles);
        //     puts("");
        // }
    }
}
