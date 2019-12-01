#pragma once

#include "Packet.h"

typedef struct
{
    Packet packet;
    int32_t turn;
}
Stream;

Stream Stream_Init(void);

Stream Stream_Flow(Stream, const Sock);
