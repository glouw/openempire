#pragma once

#include "Overview.h"
#include "Sock.h"
#include "Color.h"

#include <stdbool.h>
#include <SDL2/SDL_net.h>

#define PACKET_CONTROL_SPEED_UP ('+')
#define PACKET_CONTROL_SLOW_DOWN ('-')
#define PACKET_CONTROL_STEADY ('*')

typedef struct
{
    Overview overview[COLOR_COUNT]; // No pointers anywhere in here! Perfect for tcp...
    char control;
}
Packet;

Packet Packet_Get(const Sock);
