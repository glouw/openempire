#include "Sockets.h"

#include "Config.h"
#include "Util.h"
#include "Backup.h"

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

static Sockets AppendParity(Sockets sockets, const Color color, const Parity parity)
{
    int32_t i = 0;
    while(true)
    {
        const Parity avail = sockets.parity[color][i];
        if(Parity_IsZero(avail))
            break;
        i++;
    }
    sockets.parity[color][i] = parity;
    return sockets;
}

static Sockets Recieve(Sockets sockets)
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
                    sockets = AppendParity(sockets, (Color) i, overview.parity);
                    sockets.queue_size[i] = overview.queue_size;
                    sockets.pings[i] = overview.ping;
                    if(Overview_UsedAction(overview))
                        sockets.packet.overview[i] = overview;
                }
            }
        }
    return sockets;
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

static Sockets CalcOutOfSync(Sockets sockets)
{
    sockets.is_out_of_sync = false;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    for(int32_t j = 0; j < CONFIG_MAIN_LOOP_SPEED_FPS; j++)
        for(int32_t x = 0; x < COLOR_COUNT; x++)
        for(int32_t y = 0; y < CONFIG_MAIN_LOOP_SPEED_FPS; y++)
        {
            const Parity a = sockets.parity[x][y];
            const Parity b = sockets.parity[i][j];
            if(a.cycles == b.cycles)
                if(a.xorred != b.xorred)
                {
                    sockets.is_out_of_sync = true;
                    return sockets;
                }
        }
    return sockets;
}

Sockets Sockets_Service(Sockets sockets)
{
    sockets = Recieve(sockets);
    sockets = CountConnectedPlayers(sockets);
    sockets = CalcOutOfSync(sockets);
    return sockets;
}

static Sockets Clear(Sockets sockets)
{
    static Packet zero;
    static Parity none;
    sockets.packet = zero;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    for(int32_t j = 0; j < CONFIG_MAIN_LOOP_SPEED_FPS; j++)
        sockets.parity[i][j] = none;
    sockets.turn++;
    return sockets;
}

static int32_t GetCycleSetpoint(const Sockets sockets)
{
    int32_t setpoint = 0;
    int32_t count = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    for(int32_t j = 0; j < CONFIG_MAIN_LOOP_SPEED_FPS; j++)
    {
        const int32_t cycles = sockets.parity[i][j].cycles;
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
    for(int32_t j = 0; j < CONFIG_MAIN_LOOP_SPEED_FPS; j++)
    {
        const int32_t cycles = sockets.parity[i][j].cycles;
        if(cycles > max)
            max = cycles;
    }
    return max;
}

static int32_t GetCycleAverage(const Sockets sockets)
{
    int32_t count = 0;
    int32_t sum = 0;
    for(int32_t i = 0; i < COLOR_COUNT; i++)
    for(int32_t j = 0; j < CONFIG_MAIN_LOOP_SPEED_FPS; j++)
    {
        const Parity parity = sockets.parity[i][j];
        if(!Parity_IsZero(parity))
        {
            sum += parity.cycles;
            count++;
        }
    }
    return count == 0 ? 0 : (sum / count);
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
    {
        const int32_t average = GetCycleAverage(sockets);
        sockets.control[i] = (average < setpoint)
            ? PACKET_CONTROL_SPEED_UP
            : PACKET_CONTROL_STEADY;
    }
    return sockets;
}

static void Send(Sockets sockets, const int32_t max_cycle, const int32_t max_ping, const int32_t users, const int32_t map_power)
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
            packet.is_game_running = sockets.users_connected == users;
            packet.users_connected = sockets.users_connected;
            packet.users = users;
            packet.seed = sockets.seed;
            packet.map_power = map_power;
            packet.is_out_of_sync = sockets.is_out_of_sync;
            if(!sockets.is_stable)
                packet = Packet_ZeroOverviews(packet);
            SDLNet_TCP_Send(socket, &packet, sizeof(packet));
        }
    }
}

static Sockets CalcStability(Sockets sockets, const int32_t setpoint)
{
    sockets.is_stable = setpoint > CONFIG_SOCKETS_THRESHOLD_START;
    return sockets;
}

Sockets Sockets_Relay(Sockets sockets, const int32_t cycles, const int32_t users, const int32_t map_power)
{
    if(cycles % CONFIG_SOCKETS_SERVER_UPDATE_SPEED_CYCLES == 0)
    {
        printf("<3 %d\n", cycles);
        const int32_t setpoint = GetCycleSetpoint(sockets);
        const int32_t max_cycle = GetCycleMax(sockets);
        const int32_t max_ping = GetPingMax(sockets);
        sockets = CalcStability(sockets, setpoint);
        sockets = CalculateControlChars(sockets, setpoint);
        Send(sockets, max_cycle, max_ping, users, map_power);
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

Sockets Sockets_Panic(Sockets panic, const int32_t users_connected, const bool is_stable)
{
    if(is_stable)
    {
        if(SDLNet_CheckSockets(panic.set, CONFIG_SOCKETS_SERVER_TIMEOUT_MS)) // PANICS ARE STREAMED IN BECAUSE CLIENTS ARE NOT PREDICTABLE.
            for(int32_t i = 0; i < COLOR_COUNT; i++)
            {
                TCPsocket socket = panic.socket[i];
                if(SDLNet_SocketReady(socket))
                {
                    printf("GOT %d :: PANIC COUNT %d\n", i, panic.panic_count);
                    SDLNet_TCP_Recv(socket, panic.panic[i], sizeof(panic.panic[i]));
                    panic.panic_count++;
                }
            }
        if(panic.panic_count == users_connected) // IN A SINGLE BATCH CALL, ALL CLIENTS ARE UPDATED WITH THE INDEX THEY MUST FALL BACK WITH.
        {
            for(int32_t i = 0; i < COLOR_COUNT; i++)
            {
                int32_t index = -1;
                TCPsocket socket = panic.socket[i];
                SDLNet_TCP_Send(socket, &index, sizeof(index));
            }
            panic.panic_count = 0;
        }
        // for(int32_t i = 0; i < COLOR_COUNT; i++)
        // {
        //     for(int32_t j = 0; j < BACKUP_MAX; j++)
        //         printf("0x%016lX %5d ", parity[i][j].xorred, parity[i][j].cycles);
        //     puts("");
        // }
    }
    return panic;
}
