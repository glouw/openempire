#include "Sockets.h"

#include "Config.h"
#include "Util.h"

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
    sockets.seed = rand();
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

static void CheckParity(const Sockets sockets)
{
    if(sockets.is_stable)
        for(int32_t j = 0; j < COLOR_COUNT; j++)
        {
            const int32_t cycles_check = sockets.parity[j].cycles;
            const int32_t xorred_check = sockets.parity[j].xorred;
            for(int32_t i = 0; i < COLOR_COUNT; i++)
            {
                const int32_t cycles = sockets.parity[i].cycles;
                const int32_t xorred = sockets.parity[i].xorred;
                if(cycles == cycles_check && xorred != xorred_check)
                    Util_Bomb("SERVER - CLIENT_ID %d :: OUT OF SYNC\n", i);
            }
        }
}

Sockets Sockets_Service(Sockets sockets)
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
                    sockets.parity[i] = overview.parity;
                    sockets.queue_size[i] = overview.queue_size;
                    sockets.pings[i] = overview.ping;
                    if(Overview_UsedAction(overview))
                        sockets.packet.overview[i] = overview;
                    CheckParity(sockets);
                }
            }
        }
    return sockets;
}

static Sockets Clear(Sockets sockets)
{
    static Packet zero;
    sockets.packet = zero;
    sockets.turn++;
    return sockets;
}

static int32_t GetCycleSetpoint(const Sockets sockets)
{
    int32_t setpoint = 0;
    int32_t count = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const int32_t cycles = sockets.parity[i].cycles;
        if(cycles > 0)
        {
            setpoint += cycles;
            count++;
        }
    }
    return (count > 0) ? (setpoint / count) : 0;
}

static int32_t GetCycleMax(const Sockets sockets)
{
    int32_t max = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const int32_t cycles = sockets.parity[i].cycles;
        if(cycles > max)
            max = cycles;
    }
    return max;
}

static int32_t GetPingMax(const Sockets sockets)
{
    int32_t max = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const int32_t ping = sockets.pings[i];
        if(ping > max)
            max = ping;
    }
    return max;
}

static Sockets CalculateControlChars(Sockets sockets, const int32_t setpoint)
{
    for(int32_t i = 0; i < COLOR_COUNT; i++)
        sockets.control[i] = (sockets.parity[i].cycles < setpoint)
            ? PACKET_CONTROL_SPEED_UP
            : PACKET_CONTROL_STEADY;
    return sockets;
}

static void Print(const Sockets sockets, const int32_t setpoint, const int32_t max_ping)
{
    printf("TURN %d :: SETPOINT %d :: MAX_PING %d\n", sockets.turn, setpoint, max_ping);
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const uint64_t xorred = sockets.parity[i].xorred;
        const int32_t cycles = sockets.parity[i].cycles;
        const int32_t ping = sockets.pings[i];
        const char control = sockets.control[i];
        const char queue_size = sockets.queue_size[i];
        const char parity_symbol = sockets.is_stable ? '!' : '?';
        TCPsocket socket = sockets.socket[i];
        printf("%d :: %d :: %c :: 0x%016lX :: %c :: CYCLES %d :: QUEUE %d -> %d ms\n",
                i, socket != NULL, parity_symbol, xorred, control, cycles, queue_size, ping);
    }
}

static void Send(Sockets sockets, const int32_t max_cycle, const int32_t max_ping, const bool game_running, const int32_t users, const int32_t map_power, const int32_t users_connected)
{
    const int32_t dt_cycles = max_ping / CONFIG_MAIN_LOOP_SPEED_MS;
    const int32_t buffer = 3;
    const int32_t exec_cycle = max_cycle + dt_cycles + buffer;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        TCPsocket socket = sockets.socket[i];
        if(socket)
        {
            Packet packet = sockets.packet;
            packet.control = sockets.control[i];
            packet.turn = sockets.turn;
            packet.exec_cycle = exec_cycle;
            packet.client_id = i;
            packet.is_stable = sockets.is_stable;
            packet.game_running = game_running;
            packet.users_connected = users_connected;
            packet.users = users;
            packet.seed = sockets.seed;
            packet.map_power = map_power;
            if(!sockets.is_stable)
                packet = Packet_ZeroOverviews(packet);
            SDLNet_TCP_Send(socket, &packet, sizeof(packet));
        }
    }
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

Sockets Sockets_Relay(Sockets sockets, const int32_t cycles, const bool quiet, const int32_t users, const int32_t map_power)
{
    if(cycles % CONFIG_SOCKETS_SERVER_UPDATE_SPEED_CYCLES == 0)
    {
        const int32_t setpoint = GetCycleSetpoint(sockets);
        const int32_t max_cycle = GetCycleMax(sockets);
        const int32_t max_ping = GetPingMax(sockets);
        sockets = CalculateControlChars(sockets, setpoint);
        sockets.is_stable = setpoint > CONFIG_SOCKETS_THRESHOLD_START;
        const int32_t users_connected = CountConnectedPlayers(sockets);
        const bool game_running = users_connected == users;
        if(!quiet)
            Print(sockets, setpoint, max_ping);
        Send(sockets, max_cycle, max_ping, game_running, users, map_power, users_connected);
        return Clear(sockets);
    }
    return sockets;
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
