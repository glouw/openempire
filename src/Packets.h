#pragma once

#include "Packet.h"

#include <stdint.h>

typedef struct
{
    Packet* packet;
    int32_t a;
    int32_t b;
    int32_t max;
}
Packets;

Packets Packets_Init(void);

Packets Packets_Queue(Packets, Packet);

Packet Packets_Peek(const Packets);

Packets Packets_Dequeue(Packets, Packet*);

void Packets_Free(const Packets);

int32_t Packets_Size(const Packets);

bool Packets_Active(const Packets);

void Packets_Print(const Packets);

Packets Packets_ClearWaste(Packets, const int32_t cycles);

void Packets_FinalCheck(const Packets, const int32_t cycles);

bool Packets_MustExecute(const Packets, const int32_t cycles);
