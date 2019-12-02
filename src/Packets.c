#include "Packets.h"

#include "Util.h"

Packets Packets_Init(void)
{
    const int32_t max = 64;
    static Packets zero;
    Packets packets = zero;
    packets.packet = UTIL_ALLOC(Packet, max);
    packets.max = max;
    return packets;
}

#define WRAP(index, max) (index % max)

Packets Packets_Queue(Packets packets, Packet packet)
{
    const int32_t index = WRAP(packets.b, packets.max);
    packets.packet[index] = packet;
    packets.b++;
    if(packets.b == packets.a)
        packets.a++;
    return packets;
}

Packet Packets_Peek(const Packets packets)
{
    const int32_t index = WRAP(packets.a, packets.max);
    return packets.packet[index];
}

Packets Packets_Dequeue(Packets packets, Packet* packet)
{
    const int32_t index = WRAP(packets.a, packets.max);
    *packet = packets.packet[index];
    packets.a++;
    return packets;
}

void Packets_Free(const Packets packets)
{
    free(packets.packet);
}

int32_t Packets_Size(const Packets packets)
{
    return packets.b - packets.a;
}
