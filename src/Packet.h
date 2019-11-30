#pragma once

#include "Overview.h"
#include "Sock.h"
#include "Color.h"

#include <stdbool.h>
#include <SDL2/SDL_net.h>

typedef struct
{
    Overview overview[COLOR_COUNT];
}
Packet;

Packet Packet_Get(const Sock);
