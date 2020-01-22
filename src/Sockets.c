#include "Sockets.h"

#include "Config.h"
#include "Util.h"

#include <stdlib.h>
#include <stdbool.h>

Sockets Sockets_Init(const int32_t port, const int32_t users, const int32_t map_power)
{
    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, port);
    static Sockets zero;
    Sockets sockets = zero;
    sockets.users = users;
    sockets.self = SDLNet_TCP_Open(&ip);
    sockets.set = SDLNet_AllocSocketSet(COLOR_COUNT);
    sockets.seed = rand();
    sockets.map_power = map_power;
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

Sockets Sockets_Service(Sockets sockets, const int32_t timeout)
{
    if(SDLNet_CheckSockets(sockets.set, timeout))
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
                    sockets.cycles[i] = overview.cycles;
                    sockets.parity[i] = overview.parity;
                    sockets.queue_size[i] = overview.queue_size;
                    sockets.pings[i] = overview.ping;
                    if(Overview_UsedAction(overview))
                        sockets.packet.overview[i] = overview;
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
        const int32_t cycles = sockets.cycles[i];
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
        const int32_t cycles = sockets.cycles[i];
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
        sockets.control[i] = (sockets.cycles[i] < setpoint)
            ? PACKET_CONTROL_SPEED_UP
            : PACKET_CONTROL_STEADY;
    return sockets;
}

static void Print(const Sockets sockets, const int32_t setpoint, const int32_t max_ping)
{
    printf("TURN %d :: SETPOINT %d :: MAX_PING %d\n", sockets.turn, setpoint, max_ping);
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        const uint64_t parity = sockets.parity[i];
        const int32_t cycles = sockets.cycles[i];
        const int32_t ping = sockets.pings[i];
        const char control = sockets.control[i];
        const char queue_size = sockets.queue_size[i];
        const char parity_symbol = sockets.is_stable ? '!' : '?';
        TCPsocket socket = sockets.socket[i];
        printf("%d :: %d :: %c :: 0x%016lX :: %c :: CYCLES %d :: QUEUE %d -> %d ms\n",
                i, socket != NULL, parity_symbol, parity, control, cycles, queue_size, ping);
    }
}

static void Send(Sockets sockets, const int32_t max_cycle, const int32_t max_ping, const bool game_running)
{
    const int32_t fps = 1000 / CONFIG_MAIN_LOOP_SPEED_MS;
    const int32_t dt_cycles = (fps * max_ping) / 1000;
    const int32_t pad_cycles = 4;
    const int32_t exec_cycle = max_cycle + dt_cycles + pad_cycles;
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
            packet.users_connected = sockets.users_connected;
            packet.users = sockets.users;
            packet.seed = sockets.seed;
            packet.map_power = sockets.map_power;
            if(!sockets.is_stable)
                packet = Packet_ZeroOverviews(packet);
            SDLNet_TCP_Send(socket, &packet, sizeof(packet));
        }
    }
}

static bool ShouldRelay(const int32_t cycles, const int32_t interval)
{
    return (cycles % interval) == 0;
}

static Sockets CheckStability(Sockets sockets, const int32_t setpoint)
{
    const int32_t threshold = CONFIG_SOCKETS_THRESHOLD_START;
    sockets.is_stable = setpoint > threshold;
    return sockets;
}

static void CheckParity(const Sockets sockets)
{
    if(sockets.is_stable)
        for(int32_t j = 0; j < COLOR_COUNT; j++)
        {
            const int32_t cycles_check = sockets.cycles[j];
            const int32_t parity_check = sockets.parity[j];
            for(int32_t i = 0; i < COLOR_COUNT; i++)
            {
                const int32_t cycles = sockets.cycles[i];
                const int32_t parity = sockets.parity[i];
                if((cycles == cycles_check)
                && (parity != parity_check)) // XXX: MAKE THIS KILL THE CLIENT INSTEAD OF KILLING THE SERVER.
                    Util_Bomb("SERVER - CLIENT_ID %d :: OUT OF SYNC\n", i);
            }
        }
}

static Sockets CountConnectedPlayers(Sockets sockets)
{
    int32_t count = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    {
        TCPsocket socket = sockets.socket[i];
        if(socket != NULL)
            count++;
    }
    sockets.users_connected = count;
    return sockets;
}

static bool GetGameRunning(const Sockets sockets)
{
    return sockets.users_connected == sockets.users;
}

Sockets Sockets_Relay(Sockets sockets, const int32_t cycles, const int32_t interval, const bool quiet)
{
    if(ShouldRelay(cycles, interval))
    {
        const int32_t setpoint = GetCycleSetpoint(sockets);
        const int32_t max_cycle = GetCycleMax(sockets);
        const int32_t max_ping = GetPingMax(sockets);
        sockets = CalculateControlChars(sockets, setpoint);
        sockets = CheckStability(sockets, setpoint);
        sockets = CountConnectedPlayers(sockets);
        const bool game_running = GetGameRunning(sockets);
        if(!quiet)
            Print(sockets, setpoint, max_ping);
        CheckParity(sockets);
        Send(sockets, max_cycle, max_ping, game_running);
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

void Sockets_Ping(const Sockets ping, const int32_t timeout)
{
    if(SDLNet_CheckSockets(ping.set, timeout))
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
