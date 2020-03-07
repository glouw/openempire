#include "Packets.h"

#include "Util.h"

Packets Packets_Make(void)
{
    const int32_t max = 32;
    static Packets zero;
    Packets packets = zero;
    packets.packet = UTIL_ALLOC(Packet, max);
    packets.max = max;
    return packets;
}

Packets Packets_Queue(Packets packets, Packet packet)
{
    const int32_t index = UTIL_WRAP(packets.b, packets.max);
    packets.packet[index] = packet;
    packets.b++;
    if(UTIL_WRAP(packets.b, packets.max)
    == UTIL_WRAP(packets.a, packets.max))
        packets.a++;
    return packets;
}

Packet Packets_Peek(const Packets packets)
{
    const int32_t index = UTIL_WRAP(packets.a, packets.max);
    return packets.packet[index];
}

Packets Packets_Dequeue(Packets packets, Packet* packet)
{
    const int32_t index = UTIL_WRAP(packets.a, packets.max);
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

static bool Active(const Packets packets)
{
    return Packets_Size(packets) > 0;
}

void Packets_Print(const Packets packets)
{
    for(int32_t i = packets.a; i < packets.b; i++)
    {
        const int32_t x = UTIL_WRAP(i, packets.max);
        printf("%d ", packets.packet[x].exec_cycle);
    }
    putchar('\n');
}

static bool IsNextStale(const Packets packets, const int32_t cycles)
{
    return Active(packets) && Packets_Peek(packets).exec_cycle < cycles;
}

Packets Packets_ClearStale(Packets packets, const int32_t cycles)
{
    while(IsNextStale(packets, cycles))
    {
        Packet stale;
        packets = Packets_Dequeue(packets, &stale);
    }
    return packets;
}

bool Packets_MustExecute(const Packets packets, const int32_t cycles)
{
    return Active(packets) && Packets_Peek(packets).exec_cycle == cycles;
}

Packets Packets_Clear(Packets packets)
{
    while(Active(packets))
    {
        Packet waste;
        packets = Packets_Dequeue(packets, &waste);
    }
    return packets;
}
