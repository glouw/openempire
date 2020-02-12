#pragma once

#include "Packet.h"
#include "Parity.h"
#include "Config.h"
#include "Backup.h"

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL_net.h>

typedef struct
{
    int32_t queue_size[COLOR_COUNT];
    int32_t pings[COLOR_COUNT];
    Parity parity[COLOR_COUNT][CONFIG_MAIN_LOOP_SPEED_FPS]; // BUFFER IS LARGE ENOUGH TO SUPPORT A FULL SECOND STREAM.
    TCPsocket socket[COLOR_COUNT];
    Parity panic[COLOR_COUNT][BACKUP_MAX];
    char control[COLOR_COUNT];
    TCPsocket self;
    Packet packet;
    SDLNet_SocketSet set;
    int32_t turn;
    int32_t seed;
    int32_t users_connected;
    bool is_out_of_sync;
    bool is_stable;
    int32_t panic_count;
}
Sockets;

Sockets Sockets_Init(const int32_t port);

void Sockets_Free(Sockets);

Sockets Sockets_Service(const Sockets);

Sockets Sockets_Relay(const Sockets, const int32_t cycles, const int32_t users, const int32_t map_power);

Sockets Sockets_Accept(const Sockets);

void Sockets_Ping(const Sockets);

Sockets Sockets_Panic(const Sockets, const int32_t users_connected, const bool is_stable);

int32_t Sockets_CountConnectedPlayers(const Sockets);

bool Sockets_IsOutOfSync(const Sockets);
